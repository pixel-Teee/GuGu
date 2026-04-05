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
    self.BackGroundImageComponent.m_onPointerMove:addFunction(self.owner:getComponent("GuGu::ScriptComponent"), "OnPointerMoveBackGround")
    self.BackGroundImageComponent.m_onPointerUp:addFunction(self.owner:getComponent("GuGu::ScriptComponent"), "OnPointerUpBackGround")
end

function JoyStick:OnPointerDownBackGround(pointerData)
    --[[
        Log
    ]]
    LuaLog("man man")

    local inputManager = GuGu.InputManager.getInputManager()
    if inputManager then
        inputManager:setCaptureUIComponent(self.BackGroundImageComponent)
    end
end

function JoyStick:OnPointerMoveBackGround(pointerData)
    --[[
        Log
    ]]
    LuaLog("Go Go")
end

function JoyStick:OnPointerUpBackGround(pointerData)
    --[[
        Log
    ]]
    LuaLog("Up Up")
end

return JoyStick