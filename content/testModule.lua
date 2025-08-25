local class = require("content/Script/Common/Class")
local test = class()

function test:init(owner)
	self.owner = owner
	--print(owner)
	print("test init")

	self.owner:setName("114514")

	self.transformComponent = self.owner:getComponent("GuGu::TransformComponent")
end

function test:update(delta)
	print(tostring(delta))
end

return test