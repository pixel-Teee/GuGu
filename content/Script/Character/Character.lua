local class = require("content/Script/Common/Class")
local Character = class()

function Character:init(owner)
	self.owner = owner
    self.bIsOnGround = false

    self.verticalVelocity = 0.0  -- 垂直速度，用于跳跃和重力
    self.gravity = -9.8 * 2.0    -- 重力加速度 (可调节强度)
    self.jumpForce = 15.0         -- 跳跃力量
    self.moveSpeed = 3.0         -- 移动速度

    -- 视角旋转 (欧拉角，单位：弧度)
    self.yaw = 0.0      -- 左右旋转 (绕Y轴)
    self.pitch = 0.0    -- 上下旋转 (绕X轴)
    self.maxPitch = 89 -- 限制上下看的角度，避免翻转

    -- 初始化朝向向量
    self.forward = GuGu.math.double3.new()
    self.forward.x = 0
    self.forward.y = 0
    self.forward.z = 1
    self.right = GuGu.math.double3.new()
    self.right.x = 1
    self.right.y = 0
    self.right.z = 0

    self.bLocked = true

    --camera parameter
    self.distance = 2.5
    self.maxDistance = 15.0
    self.minDistance = 3.0
    self.zoomSpeed = 2.0

    self.player = self.owner:getParentGameObject()
    self.pitchPivot = self.owner:getChildren("PivotX")
    self.camera = self.pitchPivot:getChildren("MainCamera")
end

function Character:normalize3D(x, y, z)
    local length = math.sqrt(x * x + y * y + z * z)
    if length == 0 then return {x = 0, y = 0, z = 0} end -- 避免除零
    return {x = x / length, y = y / length, z = z / length}
end

--根据yaw和pitch更新前向和右向向量
function Character:updateVectors()
    -- 计算前向向量 (基于球面坐标)
    self.forward.x = math.sin(self.yaw * math.pi / 180.0) * math.cos(self.pitch * math.pi / 180.0)
    self.forward.y = -math.sin(self.pitch * math.pi / 180.0)
    self.forward.z = math.cos(self.yaw * math.pi / 180.0) * math.cos(self.pitch * math.pi / 180.0)
    
    -- 归一化前向向量（在水平面上投影）
    local horizontalForward = {x = self.forward.x, y = 0, z = self.forward.z}
    local len = math.sqrt(horizontalForward.x ^ 2 + horizontalForward.z ^ 2)
    if len > 0 then
        horizontalForward.x = horizontalForward.x / len
        horizontalForward.z = horizontalForward.z / len
    end
    self.forward = self:normalize3D(self.forward.x, self.forward.y, self.forward.z)
    
    -- 右向向量 = 前向向量水平分量 × 世界上向量(0,1,0)
    self.right.x = horizontalForward.z  -- 注意叉积顺序
    self.right.y = 0
    self.right.z = -horizontalForward.x
end

--处理鼠标输入，更新视角
function Character:updateView(inputManager, deltaTime)
    local mouseDelta = inputManager:getMouseDelta()
 
    -- 鼠标灵敏度
    local sensitivity = 0.4
    -- 更新旋转角度
    self.yaw = self.yaw + mouseDelta.x * sensitivity  -- 注意符号根据你的坐标系调整
    self.pitch = self.pitch + mouseDelta.y* sensitivity
    
    -- 限制上下视角，防止摄像机翻转
    if self.pitch > self.maxPitch then
        self.pitch = self.maxPitch
    elseif self.pitch < -self.maxPitch then
        self.pitch = -self.maxPitch
    end
    -- 更新朝向向量
    self:updateVectors()
    --pivot y
    local transformComponent = self.owner:getComponent("GuGu::TransformComponent")
    local newRotator = GuGu.math.Rotator.new()
    newRotator.pitch = 0
    newRotator.yaw = self.yaw --绕y轴旋转
    newRotator.roll = 0
    transformComponent:SetRotator(newRotator)
    --pivot x
    local pivotXTransformComponent = self.pitchPivot:getComponent("GuGu::TransformComponent")
    local newRotator2 = GuGu.math.Rotator.new()
    newRotator2.pitch = self.pitch --绕x轴旋转
    newRotator2.yaw = 0
    newRotator2.roll = 0
    pivotXTransformComponent:SetRotator(newRotator2)

    local cameraTransform = self.camera:getComponent("GuGu::TransformComponent")
    local originalTrans = cameraTransform:getTranslation()
    local cameraNewPos = GuGu.math.double3.new()
    cameraNewPos.x = originalTrans.x
    cameraNewPos.y = originalTrans.y
    cameraNewPos.z = -self.distance
    local collision3DComponent = self.camera:getComponent("GuGu::Collision3DComponent")
    local rayStart = cameraTransform:getGlobalTranslation()
    local rayEnd = GuGu.math.double3.new()
    rayEnd.x = rayStart.x
    rayEnd.y = rayStart.y + 10
    rayEnd.z = rayStart.z
    if collision3DComponent then
        local hitResult = GuGu.Collision3DComponent.rayTest(rayStart, rayEnd)
        if hitResult.m_bHaveResult then
            print("camera hit terrain"..tostring(hitResult.m_hitPosition.y))
            cameraNewPos.y = hitResult.m_hitPosition.y + 0.5
        end
    end
    cameraTransform:SetTranslation(cameraNewPos)
end

--【核心函数】更新移动（基于朝向）
function Character:updateMovement(inputManager, deltaTime)
    local moveInput = {x = 0 , y = 0, z = 0}
    
    -- 获取按键输入（现在移动基于角色朝向）
    if inputManager:isKeyDown(GuGu.Keys.W) then
        moveInput.x = moveInput.x + self.forward.x
        moveInput.z = moveInput.z + self.forward.z
    end
    if inputManager:isKeyDown(GuGu.Keys.S) then
        moveInput.x = moveInput.x - self.forward.x
        moveInput.z = moveInput.z - self.forward.z
    end
    if inputManager:isKeyDown(GuGu.Keys.A) then
        moveInput.x = moveInput.x - self.right.x  -- 注意：A键向左，即负右向
        moveInput.z = moveInput.z - self.right.z
    end
    if inputManager:isKeyDown(GuGu.Keys.D) then
        moveInput.x = moveInput.x + self.right.x  -- D键向右，即正右向
        moveInput.z = moveInput.z + self.right.z
    end
    
    -- 归一化对角线移动，防止斜向移动更快
    local horizontalLen = math.sqrt(moveInput.x^2 + moveInput.z^2)
    if horizontalLen > 0 then
        moveInput.x = moveInput.x / horizontalLen
        moveInput.z = moveInput.z / horizontalLen
    end
    
    -- 应用移动速度
    moveInput.x = moveInput.x * self.moveSpeed * deltaTime
    moveInput.z = moveInput.z * self.moveSpeed * deltaTime
    
    -- 处理跳跃
    if self.isOnGround and inputManager:isKeyDown(GuGu.Keys.SpaceBar) then
        self.verticalVelocity = self.jumpForce
        self.isOnGround = false
    end
    
    -- 应用重力
    if not self.isOnGround then
        self.verticalVelocity = self.verticalVelocity + self.gravity * deltaTime
        --print("verticalVelocity"..tostring(self.verticalVelocity))
    end
    moveInput.y = self.verticalVelocity * deltaTime
    
    -- 获取当前角色位置
    local transformComponent = self.player:getComponent("GuGu::TransformComponent")
    local currentPos = transformComponent:getTranslation()
    
    -- 计算新位置
    local newPos = GuGu.math.double3.new()
    newPos.x = currentPos.x + moveInput.x
    newPos.y = currentPos.y + moveInput.y
    newPos.z = currentPos.z + moveInput.z

    --print(newPos.y)
    
    -- 回退方案：直接设置位置（不推荐，会穿墙）
    transformComponent:SetTranslation(newPos)
end

function Character:update(delta)
    local inputManager = GuGu.InputManager.getInputManager()
    -- 1. 更新视角（处理鼠标）
    self:updateView(inputManager, delta)
    
    -- 2. 更新地面检测
    self:updateGroundDetection()
    
     -- 3. 更新移动（处理键盘）
     self:updateMovement(inputManager, delta) 

    local collision3DComponent = self.player:getComponent("GuGu::Collision3DComponent")
    collision3DComponent:syncToPhysics()
end

-- 改进的地面检测（用于更新isOnGround状态）
function Character:updateGroundDetection()
    local transformComponent = self.player:getComponent("GuGu::TransformComponent")
    local rayStart = transformComponent:getTranslation()
    
    -- 射线向下发射，长度略大于角色到地面的距离
    local rayEnd = GuGu.math.double3.new()
    rayEnd.x = rayStart.x
    rayEnd.y = rayStart.y - 10
    rayEnd.z = rayStart.z

    local color = GuGu.math.float3.new()
    color.x = 0.4
    color.y = 0.3
    color.z = 0.6
    local copyedRayStart = GuGu.math.float3.new()
    copyedRayStart.x = rayStart.x
    copyedRayStart.y = rayStart.y
    copyedRayStart.z = rayStart.z
    local copyedRayEnd = GuGu.math.float3.new()
    copyedRayEnd.x = rayEnd.x
    copyedRayEnd.y = rayEnd.y
    copyedRayEnd.z = rayEnd.z
    --draw ray
    GuGu.DebugDraw.drawRay(copyedRayStart, copyedRayEnd, color)
    
    local collision3DComponent = self.player:getComponent("GuGu::Collision3DComponent")
    if collision3DComponent then
        local hitResult = GuGu.Collision3DComponent.rayTest(rayStart, rayEnd)
        if hitResult.m_bHaveResult then
            local distanceToGround = rayStart.y - 0.5 - hitResult.m_hitPosition.y
            if distanceToGround < 0.3 then
                -- 如果距离地面很近，则认为在地面上
                self.isOnGround = true
                self.verticalVelocity = math.max(self.verticalVelocity, 0) -- 重置下落速度
                local newPos = GuGu.math.double3.new()
                newPos.x = rayStart.x
                newPos.y = hitResult.m_hitPosition.y + 0.5
                newPos.z = rayStart.z
                --print("hit position:"..tostring(hitResult.m_hitPosition.y))
                transformComponent:SetTranslation(newPos)
            else
                self.isOnGround = false
            end
        else
            self.isOnGround = false
        end
    end
end

function Character:multiplyQuaternions(q1, q2)
    -- q1, q2 都是包含 w, x, y, z 键的表
    local w1, x1, y1, z1 = q1.w, q1.x, q1.y, q1.z
    local w2, x2, y2, z2 = q2.w, q2.x, q2.y, q2.z
    
    local result = {
        w = w1*w2 - x1*x2 - y1*y2 - z1*z2,
        x = w1*x2 + x1*w2 + y1*z2 - z1*y2,
        y = w1*y2 - x1*z2 + y1*w2 + z1*x2,
        z = w1*z2 + x1*y2 - y1*x2 + z1*w2
    }
    
    -- 可选：归一化结果（通常旋转四元数需要保持单位长度）
    local len = math.sqrt(result.w ^ 2 + result.x ^ 2 + result.y ^ 2 + result.z ^ 2)
    if len > 0 then
        result.w = result.w / len
        result.x = result.x / len
        result.y = result.y / len
        result.z = result.z / len
    end
    
    return result
end


function Character:getQuaternionFromYawPitch(yaw, pitch)
    local halfYaw = yaw * 0.5
    local halfPitch = pitch * 0.5
    local cy = math.cos(halfYaw)
    local sy = math.sin(halfYaw)
    local cp = math.cos(halfPitch)
    local sp = math.sin(halfPitch)
    
    local w = cp * cy
    local x = sp * cy
    local y = cp * sy
    local z = sp * sy
    
    -- 返回四元数，格式为 {w, x, y, z}
    return {w = w, x = x, y = y, z = z}
end

function Character:updateMouseInput(inputManager)
    local deltaX = 0
    local deltaY = 0
    if self.bLocked == true then
        local viewportCenter = inputManager:getViewportCenter()

        local currentMousePos = inputManager:getMousePosition()

        local leftCornerPos = inputManager:getViewportLeftUpperCornerPos()
        currentMousePos.x = currentMousePos.x + leftCornerPos.x
        currentMousePos.y = currentMousePos.y + leftCornerPos.y
    
        deltaX = currentMousePos.x - viewportCenter.x
        deltaY = currentMousePos.y - viewportCenter.y

        --print("viewportCenter x : "..tostring(viewportCenter.x).."viewportCenter y : "..tostring(viewportCenter.y))
        --print("currentMousePos x : "..tostring(currentMousePos.x).."currentMousePos y : "..tostring(currentMousePos.y))
        --print("deltaX : "..tostring(deltaX).."deltaY : "..tostring(deltaY))
        inputManager:setCursorPos(viewportCenter)
    end 

    if inputManager:isKeyDown(GuGu.Keys.Escape) then
        self.bLocked = false
    end
    -- if math.abs(deltaX) < 0.03 then
    --     deltaX = 0.0
    -- end
    -- if math.abs(deltaY) < 1.5 then
    --     deltaY = 0.0
    -- end
    return deltaX, deltaY
end

return Character