local class = require("content/Script/Common/Class")
local Character = class()

function Character:init(owner)
	self.owner = owner
    self.bIsOnGround = false

    local transformComponent = self.owner:getComponent("GuGu::TransformComponent")
    local trans = transformComponent:getTranslation()
    self.groundHeight = trans.y + 0.5
end

function Character:update(delta)
    self:updateGroundDetection()

    local transformComponent = self.owner:getComponent("GuGu::TransformComponent")
    local trans = transformComponent:getTranslation()
    trans.x = trans.x + delta * 0.05
    transformComponent:SetTranslation(trans)

    local collision3DComponent = self.owner:getComponent("GuGu::Collision3DComponent")
    collision3DComponent:syncToPhysics()
end

--检测地面
function Character:updateGroundDetection()
    local transformComponent = self.owner:getComponent("GuGu::TransformComponent")
    local rayStart = transformComponent:getTranslation()
    local temp = GuGu.math.double3()
    temp.x = 0
    temp.y = -10.0
    temp.z = 0
    local rayEnd = GuGu.math.double3()
    rayEnd.x = rayStart.x + temp.x
    rayEnd.y = rayStart.y + temp.y
    rayEnd.z = rayStart.z + temp.z

    local collision3DComponent = self.owner:getComponent("GuGu::Collision3DComponent")
    if collision3DComponent then
        local hitResult = collision3DComponent:rayTest(rayStart, rayEnd)
        if hitResult then
            local hitNormal = hitResult.m_hitNormal
            local hitPosition = hitResult.m_hitPosition
            self.groundHeight = hitPosition.y
            local trans = transformComponent:getTranslation()
            trans.y = self.groundHeight + 1
            transformComponent:SetTranslation(trans)
            print(trans.y)
            --print(self.groundHeight)
        end
    end
end

return Character