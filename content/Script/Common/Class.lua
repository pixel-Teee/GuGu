local Class = {}

function Class:Constructor() --构造函数
    self.__index = self
end

function Class:Destructor()

end

function Class:extend()
    local subclass = {}
    setmetatable(subclass, self)
    self.__index = self
    subclass.super = self
    return subclass
end

return Class