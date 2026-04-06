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

    -- local inputManager = GuGu.InputManager.getInputManager()
    -- self.startScreenPosition = inputManager:getMousePosition()
    self.threshold = 5
    self.bDrag = false
    self.bFirst = true
end

function JoyStick:deInit()
    -- self.BackGroundImageComponent.m_onPointerDown:removeFunction()
    -- self.BackGroundImageComponent.m_onPointerMove:removeFunction()
    -- self.BackGroundImageComponent.m_onPointerUp:removeFunction()
end

function JoyStick:OnPointerDownBackGround(pointerData)
    LuaLog("Down Down")
    local inputManager = GuGu.InputManager.getInputManager()
    if inputManager then
        inputManager:setCaptureUIComponent(self.BackGroundImageComponent)
    end
    self.isPointerDown = true -- 标记按下
    self.startScreenPosition = pointerData.m_screenPosition
    self.bDrag = false
end

function JoyStick:OnPointerMoveBackGround(pointerData)
    LuaLog("Move Move" .. tostring(self.bDrag))
    if not self.isPointerDown then
        return
    end
    if self.bDrag then
        self:OnDragBackGround(pointerData)
    else
        -- 尚未进入拖拽状态，检查是否超过阈值
        local currentPos = pointerData.m_screenPosition
        local deltaX = self.startScreenPosition.x - currentPos.x
        local deltaY = self.startScreenPosition.y - currentPos.y
        local sqrDelta = deltaX * deltaX + deltaY * deltaY
        if sqrDelta >= self.threshold * self.threshold then
            self.bDrag = true
            -- 超过阈值后立即响应一次拖拽，使摇杆跳到正确位置
            self:OnDragBackGround(pointerData)
        end
    end
end

function JoyStick:OnPointerUpBackGround(pointerData)
    self.isPointerDown = false
    LuaLog("Up Up")
    self.bDrag = false

    local handelUITransformComponent = self.owner:getChildren("BackGround"):getChildren("Handle"):getComponent("GuGu::UITransformComponent")
    local uiAnchorData = handelUITransformComponent.m_anchorData
    if uiAnchorData then
        local offset = uiAnchorData.m_offset
        offset.left = 0
        offset.top = 0
        offset.bottom = uiAnchorData.m_offset.bottom
        offset.right = uiAnchorData.m_offset.right
        
        uiAnchorData.m_offset = offset
        handelUITransformComponent.m_anchorData = uiAnchorData
        --LuaLog("offset.left:"..tostring(offset.left).."offset.top:"..tostring(offset.top))
    end
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
        --print("background absolutePos x"..tostring(absolutePos.x).."background absolutePos y"..tostring(viewportWidthAndHeight.y - absolutePos.y))
        --print("pointerData absolutePos x"..tostring(pointerData.m_screenPosition.x).."pointerData absolutePos x"..tostring(pointerData.m_screenPosition.y))
        -- 计算背景中心点的屏幕坐标
        local centerX = absolutePos.x + radius.x
        local centerY = viewportWidthAndHeight.y - absolutePos.y - radius.y

        local input = GuGu.math.float2.new()
        input.x = (pointerData.m_screenPosition.x - centerX) / radius.x
        input.y = (pointerData.m_screenPosition.y - centerY) / radius.y
        --LuaLog("input.x:" .. tostring(input.x) .. " input.y:" .. tostring(input.y))
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