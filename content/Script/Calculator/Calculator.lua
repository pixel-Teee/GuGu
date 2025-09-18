local class = require("content/Script/Common/Class")
local Calculator = class()

local symbolTexturePath = {
	["1"] = "content/Calculator/texture_one.json",
	["2"] = "content/Calculator/texture_two.json",
	["3"] = "content/Calculator/texture_three.json",
	["4"] = "content/Calculator/texture_four.json",
	["5"] = "content/Calculator/texture_five.json",
	["6"] = "content/Calculator/texture_six.json",
	["7"] = "content/Calculator/texture_seven.json",
	["8"] = "content/Calculator/texture_eight.json",
	["9"] = "content/Calculator/texture_nine.json",
	["0"] = "content/Calculator/texture_zero.json",
	--["."] = "content/Calculator/texture_dot.json",
	--["+"] = "content/Calculator/texture_plus.json",
	--["-"] = "content/Calculator/texture_mius.json",
	--["*"] = "content/Calculator/texture_times.json",
	--["/"] = "content/Calculator/texture_division.json",
	--["="] = "content/Calculator/texture_equal.json"
}

function Calculator:init(owner)
	self.owner = owner

	self.textComponent = self.owner:getChildren("Text_FPS"):getComponent("GuGu::TextComponent")

	self.frameCount = 0
	self.elapsedTime = 0

	local currentLevel = self.owner:getCurrentLevel()
	self.centerBorder = self.owner:getCurrentLevel():getGameObject("CenterBorder")
	if currentLevel then
		self.screen = currentLevel:createGameObject("screent_1")
		self.symbolGameObjects = {}
		self.screen:addComponentFromName("GuGu::UITransformComponent")
		if self.screen then
			self.centerBorder:addChildren(self.screen)
			local textComponent = self.screen:addComponentFromName("GuGu::TextComponent");
			textComponent:setText("114514")
			print("add text component successful")
			--local imageComponent = self.screen:addComponentFromName("GuGu::ImageComponent");
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
					--self.screen:addChildren(symbolGameObject)
					local uiAnchorData = uiTransformComponent.m_anchorData
					if uiAnchorData then
						local anchors = uiAnchorData.m_anchors
						local offset = uiAnchorData.m_offset
						local mini = anchors.m_minimum
						local maxi = anchors.m_maximum
						mini.x = 1.0
						mini.y = 0.5
						maxi.x = 1.0
						maxi.y = 0.5
						anchors.m_minimum = mini
						anchors.m_maximum = maxi
						offset.top = 0
						offset.left = leftOffset
						offset.right = 40
						offset.bottom = 40
						uiAnchorData.m_anchors = anchors
						uiAnchorData.m_offset = offset
						uiTransformComponent.m_anchorData = uiAnchorData
						leftOffset = leftOffset + 40
					end
					--table.insert(self.symbolGameObjects, symbolGameObject)
					self.symbolGameObjects[key] = symbolGameObject
				end
			end
			currentLevel:refreshLevel() --refresh editor
		end
	end

	for key, value in pairs(self.symbolGameObjects) do
		local buttonComponent = self.owner:getCurrentLevel():getGameObject(key):getComponent("GuGu::ButtonComponent")
		if buttonComponent then
			buttonComponent.m_onClicked:addFunction(self.owner:getComponent("GuGu::ScriptComponent"), "click_"..key)
		end 
	end
	local buttonComponent = self.owner:getCurrentLevel():getGameObject("Clear"):getComponent("GuGu::ButtonComponent")
	if buttonComponent then
		buttonComponent.m_onClicked:addFunction(self.owner:getComponent("GuGu::ScriptComponent"), "click_clear")
	end 
	self.symbolStack = {}
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

function Calculator:click_1()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["1"])
		table.insert(self.symbolStack, 1)
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_2()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["1"])
		table.insert(self.symbolStack, 2)
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_3()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["3"])
		table.insert(self.symbolStack, 3)
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_4()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["4"])
		table.insert(self.symbolStack, 4)
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_5()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["5"])
		table.insert(self.symbolStack, 5)
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_6()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["6"])
		table.insert(self.symbolStack, 6)
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_7()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["7"])
		table.insert(self.symbolStack, 7)
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_8()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["8"])
		table.insert(self.symbolStack, 8)
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_9()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["9"])
		table.insert(self.symbolStack, 9)
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_0()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["0"])
		table.insert(self.symbolStack, 0)
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_clear()
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["0"])
		self.symbolStack = {}
		self.screen:clearChildrens()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:refreshScreen()
	if self.screen then
		self.screen:clearChildrens()

		for i = #self.symbolStack, 1, -1 do
			local symbol = self.symbolStack[i]
			self.screen:addChildren(self.symbolGameObjects[tostring(symbol)])

			local uiTransformComponent = self.symbolGameObjects[tostring(symbol)]:getComponent("GuGu::UITransformComponent")

			local uiAnchorData = uiTransformComponent.m_anchorData
			if uiAnchorData then
				local anchors = uiAnchorData.m_anchors
				local offset = uiAnchorData.m_offset
				local mini = anchors.m_minimum
				local maxi = anchors.m_maximum
				mini.x = 1.0
				mini.y = 0.5
				maxi.x = 1.0
				maxi.y = 0.5
				anchors.m_minimum = mini
				anchors.m_maximum = maxi
				offset.top = 20
				offset.left = -(#self.symbolStack - i) * 40 - 20
				offset.right = 40
				offset.bottom = 40
				uiAnchorData.m_anchors = anchors
				uiAnchorData.m_offset = offset
				uiTransformComponent.m_anchorData = uiAnchorData
			end
		end
	end
end

return Calculator