local class = require("content/Script/Common/Class")
local Character = class()

function Character:init(owner)
	self.owner = owner
    self.bIsOnGround = false

    self.verticalVelocity = 0.0  -- 垂直速度，用于跳跃和重力
    self.gravity = -9.8 * 2.0    -- 重力加速度 (可调节强度)
    self.jumpForce = 5.0         -- 跳跃力量
    self.moveSpeed = 3.0         -- 移动速度

    -- 视角旋转 (欧拉角，单位：弧度)
    self.yaw = 0.0      -- 左右旋转 (绕Y轴)
    self.pitch = 0.0    -- 上下旋转 (绕X轴)
    self.maxPitch = math.rad(89) -- 限制上下看的角度，避免翻转

    -- 初始化朝向向量
    self.forward = GuGu.math.double3.new()
    self.forward.x = 0
    self.forward.y = 0
    self.forward.z = 1
    self.right = GuGu.math.double3.new()
    self.right.x = 0
    self.right.y = 0
    self.right.z = 0

    self.bLocked = true

    -- local transformComponent = self.owner:getComponent("GuGu::TransformComponent")
    -- local trans = transformComponent:getTranslation()
    -- self.groundHeight = trans.y + 0.5

    self:updateVectors()
end

--【核心函数】根据yaw和pitch更新前向和右向向量
function Character:updateVectors()
    -- 计算前向向量 (基于球面坐标)
    self.forward.x = math.sin(self.yaw)
    self.forward.y = -math.sin(self.pitch) * math.cos(self.yaw)
    self.forward.z = math.cos(self.pitch) * math.cos(self.yaw)
    
    -- 归一化前向向量（在水平面上投影）
    local horizontalForward = {x = self.forward.x, y = 0, z = self.forward.z}
    local len = math.sqrt(horizontalForward.x ^ 2 + horizontalForward.z ^ 2)
    if len > 0 then
        horizontalForward.x = horizontalForward.x / len
        horizontalForward.z = horizontalForward.z / len
    end
    
    -- 右向向量 = 前向向量水平分量 × 世界上向量(0,1,0)
    self.right.x = horizontalForward.z  -- 注意叉积顺序
    self.right.y = 0
    self.right.z = -horizontalForward.x
    
    -- 可选：也可以在这里计算真正的上向量，用于复杂摄像机
end

--【核心函数】处理鼠标输入，更新视角
function Character:updateView(inputManager, deltaTime)
    -- 获取鼠标位移（假设inputManager能提供）
    -- 注意：这里需要你的输入系统支持获取鼠标增量
    local mouseDeltaX, mouseDeltaY = self:updateMouseInput(inputManager)
 
    -- 鼠标灵敏度
    local sensitivity = 0.002
    
    -- 更新旋转角度
    self.yaw = self.yaw + mouseDeltaX * sensitivity  -- 注意符号根据你的坐标系调整
    self.pitch = self.pitch + mouseDeltaY * sensitivity
    
    -- 限制上下视角，防止摄像机翻转
    if self.pitch > self.maxPitch then
        self.pitch = self.maxPitch
    elseif self.pitch < -self.maxPitch then
        self.pitch = -self.maxPitch
    end
    
    -- 更新朝向向量
    self:updateVectors()
    
    -- 【关键】将旋转应用到角色的变换组件（如果是第一人称，通常只旋转摄像机或角色模型）
    -- 这里我们更新角色的整体朝向（如果角色模型和摄像机是一体的）
    local transformComponent = self.owner:getComponent("GuGu::TransformComponent")
    -- 将四元数旋转应用到变换组件（你需要根据引擎API调整）
    -- 例如：transformComponent:setRotation(从yaw和pitch创建的四元数)
    local rotationQuat = self:getQuaternionFromYawPitch(self.yaw, self.pitch)
    local newQuat = GuGu.math.dquat.new()
    newQuat.w = rotationQuat.w
    newQuat.x = rotationQuat.x
    newQuat.y = rotationQuat.y
    newQuat.z = rotationQuat.z
    transformComponent:SetRotationQuat(newQuat)
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
    end
    moveInput.y = self.verticalVelocity * deltaTime
    
    -- 获取当前角色位置
    local transformComponent = self.owner:getComponent("GuGu::TransformComponent")
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

    local collision3DComponent = self.owner:getComponent("GuGu::Collision3DComponent")
    collision3DComponent:syncToPhysics()
end

-- 改进的地面检测（用于更新isOnGround状态）
function Character:updateGroundDetection()
    local transformComponent = self.owner:getComponent("GuGu::TransformComponent")
    local rayStart = transformComponent:getTranslation()
    
    -- 射线向下发射，长度略大于角色到地面的距离
    local rayEnd = GuGu.math.double3.new()
    rayEnd.x = rayStart.x
    rayEnd.y = rayStart.y - 1.1
    rayEnd.z = rayStart.z
    
    local collision3DComponent = self.owner:getComponent("GuGu::Collision3DComponent")
    if collision3DComponent then
        local hitResult = GuGu.Collision3DComponent.rayTest(rayStart, rayEnd)
        if hitResult then
            local distanceToGround = rayStart.y - hitResult.m_hitPosition.y
            -- 如果距离地面很近，则认为在地面上
            if distanceToGround < 1.05 and distanceToGround > 0.95 then
                self.isOnGround = true
                self.verticalVelocity = math.max(self.verticalVelocity, 0) -- 重置下落速度
            else
                self.isOnGround = false
            end
        else
            self.isOnGround = false
        end
    end
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
    
        deltaX = currentMousePos.x - viewportCenter.y
        deltaY = currentMousePos.x - viewportCenter.y

        print(viewportCenter.x)
        print(viewportCenter.y)
        --inputManager:setCursorPos(viewportCenter)
    end 

    if inputManager:isKeyDown(GuGu.Keys.Escape) then
        self.bLocked = false
    end
    return deltaX, deltaY
end

return Character