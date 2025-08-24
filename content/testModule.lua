local class = require("content/Script/Common/Class")
local test = class()

function test:init(owner)
	self.owner = owner

	print("test init")

	self.owner:setName("114514")

end

return test