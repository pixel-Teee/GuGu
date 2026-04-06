local class = require("Common/Class")
local JoyStick = class()
local EventSystem = require("Event/EventSystem")

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

    local inputManager = GuGu.InputManager.getInputManager()
    self.startScreenPosition = inputManager:getMousePosition()
    self.threshold = 5
    self.bDrag = false
end

function JoyStick:deInit()
    -- self.BackGroundImageComponent.m_onPointerDown:removeFunction()
    -- self.BackGroundImageComponent.m_onPointerMove:removeFunction()
    -- self.BackGroundImageComponent.m_onPointerUp:removeFunction()
end

function JoyStick:OnPointerDownBackGround(pointerData)
    local inputManager = GuGu.InputManager.getInputManager()
    if inputManager then
        inputManager:setCaptureUIComponent(self.BackGroundImageComponent)
    end
    self.startScreenPosition = pointerData.m_screenPosition
end

function JoyStick:OnPointerMoveBackGround(pointerData)
    if self.bDrag then
        self:OnDragBackGround(pointerData)
    else
        local currentScreenPosition = pointerData.m_screenPosition
        local deltaX = self.startScreenPosition.x - currentScreenPosition.x
        local deltaY = self.startScreenPosition.y - currentScreenPosition.y
        if self.threshold <= deltaX * deltaX + deltaY + deltaY then
            self.bDrag = true
        end
    end
end

function JoyStick:OnPointerUpBackGround(pointerData)
    LuaLog("Up Up")
    self.bDrag = false
end

function JoyStick:OnDragBackGround(pointerData)
    local uiTransformComponent = self.owner:getChildren("BackGround"):getComponent("GuGu::UITransformComponent")
    local inputManager = GuGu.InputManager.getInputManager()
    if uiTransformComponent then
        local viewportWidthAndHeight = inputManager:getViewportWidthAndHeight()
        local radius = GuGu.math.float2.new()
        radius.x = uiTransformComponent.m_localSize.x / 2.0
        radius.y = uiTransformComponent.m_localSize.y / 2.0

        local absolutePos = uiTransformComponent:getGlobalTranslation()
        print("background absolutePos x"..tostring(absolutePos.x).."background absolutePos y"..tostring(viewportWidthAndHeight.y - absolutePos.y))
        print("pointerData absolutePos x"..tostring(pointerData.m_screenPosition.x).."pointerData absolutePos x"..tostring(pointerData.m_screenPosition.y))
        -- 计算背景中心点的屏幕坐标
        local centerX = absolutePos.x + radius.x
        local centerY = viewportWidthAndHeight.y - absolutePos.y - radius.y

        local input = GuGu.math.float2.new()
        input.x = (pointerData.m_screenPosition.x - centerX) / radius.x
        input.y = (pointerData.m_screenPosition.y - centerY) / radius.y
        LuaLog("input.x:" .. tostring(input.x) .. " input.y:" .. tostring(input.y))
        local magnitude = math.sqrt(input.x * input.x + input.y * input.y)
        --处理半径内的拖动和边界限制
        local normalizedInput = GuGu.math.float2.new()
        if magnitude > 1.0 then
            --超出圆盘边界，限制在边缘
            normalizedInput.x = input.x / magnitude
            normalizedInput.y = input.y / magnitude
        else
            normalizedInput.x = input.x
            normalizedInput.y = input.y
        end

        local handelUITransformComponent = self.owner:getChildren("BackGround"):getChildren("Handle"):getComponent("GuGu::UITransformComponent")
        local uiAnchorData = handelUITransformComponent.m_anchorData
        if uiAnchorData then
            local offset = uiAnchorData.m_offset
            offset.left = normalizedInput.x * radius.x
            offset.top = normalizedInput.y * radius.y
            offset.bottom = uiAnchorData.m_offset.bottom
            offset.right = uiAnchorData.m_offset.right
            
            uiAnchorData.m_offset = offset
            handelUITransformComponent.m_anchorData = uiAnchorData
            --LuaLog("offset.left:"..tostring(offset.left).."offset.top:"..tostring(offset.top))
        end
        local moveInput = {
            x = normalizedInput.x,
            y = 0,
            z = -normalizedInput.y
        }
        EventSystem:Send("JoyStickMovement", nil, moveInput, self.deltaTime)
    end
end

function JoyStick:update(delta)
    self.deltaTime = delta
end

return JoyStick