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

    -- local transformComponent = self.owner:getComponent("GuGu::TransformComponent")
    -- local trans = transformComponent:getTranslation()
    -- self.groundHeight = trans.y + 0.5

    self:updateVectors()
end

--【核心函数】根据yaw和pitch更新前向和右向向量
function Character:updateVectors()
    -- 计算前向向量 (基于球面坐标)
    self.forward.x = math.cos(self.yaw) * math.cos(self.pitch)
    self.forward.y = math.sin(self.pitch)
    self.forward.z = math.sin(self.yaw) * math.cos(self.pitch)
    
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
    local mouseDelta = inputManager:getMouseDelta()
    local mouseDeltaX = mouseDelta.x or 0
    local mouseDeltaY = mouseDelta.y or 0
    
    -- 鼠标灵敏度
    local sensitivity = 0.002
    
    -- 更新旋转角度
    self.yaw = self.yaw - mouseDeltaX * sensitivity  -- 注意符号根据你的坐标系调整
    self.pitch = self.pitch - mouseDeltaY * sensitivity
    
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
        moveInput.x = moveInput.x + self.right.x  -- 注意：A键向左，即负右向
        moveInput.z = moveInput.z + self.right.z
    end
    if inputManager:isKeyDown(GuGu.Keys.D) then
        moveInput.x = moveInput.x - self.right.x  -- D键向右，即正右向
        moveInput.z = moveInput.z - self.right.z
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
    -- 1. 创建绕Y轴旋转的四元数 (yaw， 影响左右朝向)
    local halfYaw = yaw * 0.5
    local cosYaw = math.cos(halfYaw)
    local sinYaw = math.sin(halfYaw)
    -- 绕Y轴(0,1,0)旋转的四元数公式: (cosYaw, 0, sinYaw, 0)
    local quatYaw = {w = cosYaw, x = 0.0, y = sinYaw, z = 0.0}

    -- 2. 创建绕X轴旋转的四元数 (pitch， 影响上下视角)
    local halfPitch = pitch * 0.5
    local cosPitch = math.cos(halfPitch)
    local sinPitch = math.sin(halfPitch)
    -- 绕X轴(1,0,0)旋转的四元数公式: (cosPitch, sinPitch, 0, 0)
    local quatPitch = {w = cosPitch, x = sinPitch, y = 0.0, z = 0.0}

    -- 3. 按顺序组合：先yaw，后pitch （四元数乘法：q_result = q_pitch * q_yaw）
    -- 注意乘法顺序：由于我们采用“局部旋转”思想（先转yaw，再在当前基础上转pitch），
    -- 所以是后发生的旋转（pitch）乘以前面的旋转（yaw）。
    local qy = quatYaw
    local qp = quatPitch
    local combinedQuat = {
        w = qp.w * qy.w - qp.x * qy.x - qp.y * qy.y - qp.z * qy.z,
        x = qp.w * qy.x + qp.x * qy.w + qp.y * qy.z - qp.z * qy.y,
        y = qp.w * qy.y - qp.x * qy.z + qp.y * qy.w + qp.z * qy.x,
        z = qp.w * qy.z + qp.x * qy.y - qp.y * qy.x + qp.z * qy.w
    }
    -- 可选：归一化四元数，确保其长度为1（对于旋转四元数，这通常是必需的）
    local len = math.sqrt(combinedQuat.w^2 + combinedQuat.x^2 + combinedQuat.y^2 + combinedQuat.z^2)
    if len > 0 then
        combinedQuat.w = combinedQuat.w / len
        combinedQuat.x = combinedQuat.x / len
        combinedQuat.y = combinedQuat.y / len
        combinedQuat.z = combinedQuat.z / len
    end
    return combinedQuat
end

return Character