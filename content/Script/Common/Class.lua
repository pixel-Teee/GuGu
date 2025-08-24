local function class(super)
	local cls = {}
	cls.__index = cls
	cls.super = super

	if super then
		setmetatable(cls, {__index = super})
	end

	-- 构造函数
	function cls.new(...)
		local instance = setmetatable({}, cls) --对一个空表，设置元表为类表，并返回
		if cls.init then --判断这个类表有没有init函数
			cls.init(instance, ...)
		end
		return instance --返回实例表
	end

	print("register class success!")

	return cls
end

return class

