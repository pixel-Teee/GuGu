local class = require("Common/Class")
local JoyStick = class()

local JoyStickType = {
    Fixed = 1,
    Floating = 2,
    Dynamic = 3
}

--[[
    UI
]]
function JoyStick:init(owner)
	self.owner = owner
    
    self.BackGroundImageComponent = self.owner:getChildren("BackGround"):getComponent("GuGu::ImageComponent")

    self.HandleImageComponent = self.owner:getChildren("BackGround"):getChildren("Handle"):getComponent("GuGu::ImageComponent")

    self.BackGroundImageComponent.m_onPointerDown:addFunction(self.owner:getComponent("GuGu::ScriptComponent"), "OnPointerDownBackGround")
end

function JoyStick:OnPointerDownBackGround(pointerData)
    --[[
        Log
    ]]
    LuaLog("man man")
end

return JoyStick