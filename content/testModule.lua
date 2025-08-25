local class = require("content/Script/Common/Class")
local test = class()

function test:init(owner)
	self.owner = owner
	--print(owner)
	print("test init")

	self.owner:setName("114514")

	self.transformComponent = self.owner:getComponent("GuGu::TransformComponent")

	local trans = GuGu.math.double3()
	trans.x = 0.05
	trans.y = 0
	trans.z = 0
	
	self.transformComponent:SetTranslation(trans)
end

function test:update(delta)
	--print(tostring(delta))
	local oriTrans = self.transformComponent:getTranslation()
	print(tostring(oriTrans.x))
	local trans = GuGu.math.double3()
	trans.x = oriTrans.x + delta * 20
	trans.y = 0
	trans.z = 0
	self.transformComponent:SetTranslation(trans)
end

return test