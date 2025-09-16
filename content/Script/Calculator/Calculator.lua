local class = require("content/Script/Common/Class")
local Calculator = class()

local symbolTexturePath = {
	["one"] = "content/Calculator/texture_one.json",
	["two"] = "content/Calculator/texture_two.json"
}

function Calculator:init(owner)
	self.owner = owner

	self.textComponent = self.owner:getChildren("Text_FPS"):getComponent("GuGu::TextComponent")

	self.frameCount = 0
	self.elapsedTime = 0

	local currentLevel = self.owner:getCurrentLevel()
	self.centerBorder = self.owner:getCurrentLevel():getGameObject("CenterBorder")
	if currentLevel then
		local newGameObject = currentLevel:createGameObject("screent_1")
		self.symbolGameObjects = {}
		newGameObject:addComponentFromName("GuGu::UITransformComponent")
		if newGameObject then
			self.centerBorder:addChildren(newGameObject)
			local textComponent = newGameObject:addComponentFromName("GuGu::TextComponent");
			textComponent:setText("114514")
			print("add text component successful")
			--local imageComponent = newGameObject:addComponentFromName("GuGu::ImageComponent");
			--local oneTexture = self.owner:getWorld():loadTexture(symbolTexturePath["one"])
			--if oneTexture then
			--	imageComponent:setTextureAsset(oneTexture)
			--end
			local leftOffset = 0
			for key, value in pairs(symbolTexturePath) do
				print("traverse symbol texture path array")
				local symbolGameObject = currentLevel:createGameObject("key_symbol"..key)
				if symbolGameObject then
					local imageComponent = symbolGameObject:addComponentFromName("GuGu::ImageComponent")
					local uiTransformComponent = symbolGameObject:addComponentFromName("GuGu::UITransformComponent")
					if imageComponent then
						local texture = self.owner:getWorld():loadTexture(value)
						if texture then
							imageComponent:setTextureAsset(texture)
						end
					end
					newGameObject:addChildren(symbolGameObject)
					local uiAnchorData = uiTransformComponent.m_anchorData
					if uiAnchorData then
						local anchors = uiAnchorData.m_anchors
						local offset = uiAnchorData.m_offset
						local mini = anchors.m_minimum
						local maxi = anchors.m_maximum
						mini.x = 0.5
						mini.y = 0.5
						maxi.x = 0.5
						maxi.y = 0.5
						anchors.m_minimum = mini
						anchors.m_maximum = maxi
						offset.left = leftOffset
						offset.right = 40
						offset.bottom = 40
						uiAnchorData.m_anchors = anchors
						uiAnchorData.m_offset = offset
						uiTransformComponent.m_anchorData = uiAnchorData
						leftOffset = leftOffset + 40
					end
					table.insert(self.symbolGameObjects, symbolGameObject)
				end
			end
			currentLevel:refreshLevel() --refresh editor
		end
	end
end

function Calculator:update(delta)
	self.elapsedTime = self.elapsedTime + delta
	self.frameCount = self.frameCount + 1
	if self.elapsedTime > 1.0 then
		local fps = self.frameCount / self.elapsedTime
		self.elapsedTime = 0
		self.frameCount = 0
		self.textComponent:setText("帧率"..string.format("%.2f", fps))
	end
end

return Calculator