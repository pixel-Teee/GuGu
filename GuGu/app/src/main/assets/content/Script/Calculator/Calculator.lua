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
	["."] = "content/Calculator/texture_dot.json",
	["+"] = "content/Calculator/texture_plus.json",
	["-"] = "content/Calculator/texture_mius.json",
	["*"] = "content/Calculator/texture_times.json",
	["/"] = "content/Calculator/texture_division.json",
	["="] = "content/Calculator/texture_equal.json"
}

local defaultWhiteTexturePath = "content/white.json"
local defaultWhiteTexture = nil

function Calculator:init(owner)
	self.owner = owner

	for key, value in pairs(symbolTexturePath) do
		local gameObject = self.owner:getCurrentLevel():getGameObject(key)
		if gameObject ~= nil then
			local buttonComponent = gameObject:getComponent("GuGu::ButtonComponent")
			if buttonComponent then
				buttonComponent.m_onClicked:addFunction(self.owner:getComponent("GuGu::ScriptComponent"), "click_"..key)
			end 
		end
	end

	self.textComponent = self.owner:getChildren("Text_FPS"):getComponent("GuGu::TextComponent")

	self.frameCount = 0
	self.elapsedTime = 0

	local currentLevel = self.owner:getCurrentLevel()
	self.centerBorder = self.owner:getCurrentLevel():getGameObject("CenterBorder")
	self.symbolTextureAssets = {}
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
			for key, value in pairs(symbolTexturePath) do
				print("traverse symbol texture path array")
				local texture = self.owner:getWorld():loadTexture(value)
				-- if texture then
				-- 	imageComponent:setTextureAsset(texture)
				-- end
				--table.insert(self.symbolTextureAssets, texture)
				self.symbolTextureAssets[key] = texture
				--local symbolGameObject = currentLevel:createGameObject("key_symbol"..key)
				-- if symbolGameObject then
				-- 	local imageComponent = symbolGameObject:addComponentFromName("GuGu::ImageComponent")
				-- 	local uiTransformComponent = symbolGameObject:addComponentFromName("GuGu::UITransformComponent")
				-- 	if imageComponent then
				-- 		local texture = self.owner:getWorld():loadTexture(value)
				-- 		-- if texture then
				-- 		-- 	imageComponent:setTextureAsset(texture)
				-- 		-- end
				-- 		table.insert(self.symbolTextureAssets, texture)
				-- 	end
				-- 	--self.screen:addChildren(symbolGameObject)
				-- 	local uiAnchorData = uiTransformComponent.m_anchorData
				-- 	if uiAnchorData then
				-- 		local anchors = uiAnchorData.m_anchors
				-- 		local offset = uiAnchorData.m_offset
				-- 		local mini = anchors.m_minimum
				-- 		local maxi = anchors.m_maximum
				-- 		mini.x = 1.0
				-- 		mini.y = 0.5
				-- 		maxi.x = 1.0
				-- 		maxi.y = 0.5
				-- 		anchors.m_minimum = mini
				-- 		anchors.m_maximum = maxi
				-- 		offset.top = 0
				-- 		offset.left = leftOffset
				-- 		offset.right = 40
				-- 		offset.bottom = 40
				-- 		uiAnchorData.m_anchors = anchors
				-- 		uiAnchorData.m_offset = offset
				-- 		uiTransformComponent.m_anchorData = uiAnchorData
				-- 		leftOffset = leftOffset + 40
				-- 	end
				-- 	--table.insert(self.symbolGameObjects, symbolGameObject)
				-- 	self.symbolGameObjects[key] = symbolGameObject
				-- end
			end
			currentLevel:refreshLevel() --refresh editor
		end
	end

	self.normalGameObjects = {}
	if self.screen then
		self.screen:clearChildrens()

		for i = 1, 9 do
			local normalGameObject = currentLevel:createGameObject("normal"..tostring(i))
			normalGameObject:addComponentFromName("GuGu::ImageComponent")
			normalGameObject:addComponentFromName("GuGu::UITransformComponent")

			self.screen:addChildren(normalGameObject)
			
			local uiTransformComponent = normalGameObject:getComponent("GuGu::UITransformComponent")
			local imageComponent = normalGameObject:getComponent("GuGu::ImageComponent")

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
				offset.left = -i * 40 - 20
				offset.right = 40
				offset.bottom = 40
				uiAnchorData.m_anchors = anchors
				uiAnchorData.m_offset = offset
				uiTransformComponent.m_anchorData = uiAnchorData
			end

			if imageComponent then
				local color = imageComponent.m_color
				color.r = 1.0
				color.g = 1.0
				color.b = 1.0
				color.a = 0.0
				imageComponent.m_color = color
			end

			table.insert(self.normalGameObjects, normalGameObject)
		end
	end

	for key, value in pairs(symbolTexturePath) do
		local gameObject = self.owner:getCurrentLevel():getGameObject(key)
		if gameObject ~= nil then
			local buttonComponent = gameObject:getComponent("GuGu::ButtonComponent")
			if buttonComponent then
				buttonComponent.m_onClicked:addFunction(self.owner:getComponent("GuGu::ScriptComponent"), "click_"..key)
			end 
		end
	end
	local buttonComponent = self.owner:getCurrentLevel():getGameObject("Clear"):getComponent("GuGu::ButtonComponent")
	if buttonComponent then
		buttonComponent.m_onClicked:addFunction(self.owner:getComponent("GuGu::ScriptComponent"), "click_clear")
	end 
	buttonComponent = self.owner:getCurrentLevel():getGameObject("Equals"):getComponent("GuGu::ButtonComponent")
	if buttonComponent then
		buttonComponent.m_onClicked:addFunction(self.owner:getComponent("GuGu::ScriptComponent"), "click_equals")
	end
	buttonComponent = self.owner:getCurrentLevel():getGameObject("Plus"):getComponent("GuGu::ButtonComponent")
	if buttonComponent then
		buttonComponent.m_onClicked:addFunction(self.owner:getComponent("GuGu::ScriptComponent"), "click_plus")
	end
	buttonComponent = self.owner:getCurrentLevel():getGameObject("Minus"):getComponent("GuGu::ButtonComponent")
	if buttonComponent then
		buttonComponent.m_onClicked:addFunction(self.owner:getComponent("GuGu::ScriptComponent"), "click_minus")
	end
	buttonComponent = self.owner:getCurrentLevel():getGameObject("Times"):getComponent("GuGu::ButtonComponent")
	if buttonComponent then
		buttonComponent.m_onClicked:addFunction(self.owner:getComponent("GuGu::ScriptComponent"), "click_times")
	end
	buttonComponent = self.owner:getCurrentLevel():getGameObject("Division"):getComponent("GuGu::ButtonComponent")
	if buttonComponent then
		buttonComponent.m_onClicked:addFunction(self.owner:getComponent("GuGu::ScriptComponent"), "click_division")
	end
	
	self.symbolStack = {}
	defaultWhiteTexture = self.owner:getWorld():loadTexture(defaultWhiteTexturePath)
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
	print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["1"])
		table.insert(self.symbolStack, "1")
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_2()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["1"])
		table.insert(self.symbolStack, "2")
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_3()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["3"])
		table.insert(self.symbolStack, "3")
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_4()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["4"])
		table.insert(self.symbolStack, "4")
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_5()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["5"])
		table.insert(self.symbolStack, "5")
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_6()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["6"])
		table.insert(self.symbolStack, "6")
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_7()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["7"])
		table.insert(self.symbolStack, "7")
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_8()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["8"])
		table.insert(self.symbolStack, "8")
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_9()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["9"])
		table.insert(self.symbolStack, "9")
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_0()
	--print("call lua function")
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["0"])
		table.insert(self.symbolStack, "0")
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_equals()
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["0"])
		local expression = ""
		for key, value in pairs(self.symbolStack) do
			expression = expression .. value
		end
		print("expression:", expression)
		local func, err = load("return "..expression)
		if func then
			local result = func()
			print("result:"..tostring(result))
			local str = tostring(result)
			self.symbolStack = {}
			for i = 1, #str do
				table.insert(self.symbolStack, str:sub(i, i))
			end
		else
			print(err)
			self:click_clear()
		end
		for key, value in pairs(self.normalGameObjects) do
			local imageComponent = value:getComponent("GuGu::ImageComponent")
			if imageComponent then
				local color = imageComponent.m_color
				color.r = 1.0
				color.g = 1.0
				color.b = 1.0
				color.a = 0.0
				imageComponent.m_color = color
				imageComponent:setTextureAsset(defaultWhiteTexture)
			end
		end
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_plus()
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["0"])
		table.insert(self.symbolStack, "+")
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_minus()
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["0"])
		table.insert(self.symbolStack, "-")
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_times()
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["0"])
		table.insert(self.symbolStack, "*")
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_division()
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["0"])
		table.insert(self.symbolStack, "/")
		self:refreshScreen()
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:click_clear()
	if self.screen then
		--self.screen:addChildren(self.symbolGameObjects["0"])
		self.symbolStack = {}
		--self.screen:clearChildrens()
		for key, value in pairs(self.normalGameObjects) do
			local imageComponent = value:getComponent("GuGu::ImageComponent")
			if imageComponent then
				local color = imageComponent.m_color
				color.r = 1.0
				color.g = 1.0
				color.b = 1.0
				color.a = 0.0
				imageComponent.m_color = color
				imageComponent:setTextureAsset(defaultWhiteTexture)
			end
		end
		self.owner:getCurrentLevel():refreshLevel()
	end
end

function Calculator:refreshScreen()
	if self.screen then
		for i = #self.symbolStack, 1, -1 do
			local symbol = self.symbolStack[i]
			
			local normalGameObject = self.normalGameObjects[#self.symbolStack - i + 1]
			print("traverse game object index, %s", tostring(#self.symbolStack - i + 1))
			if normalGameObject then
				local imageComponent = normalGameObject:getComponent("GuGu::ImageComponent")
				if imageComponent then
					local color = imageComponent.m_color
					color.a = 1.0
					imageComponent.m_color = color
					print("set texture asset symbol %s", tostring(symbol))
					imageComponent:setTextureAsset(self.symbolTextureAssets[symbol])
				end
			end
		end
	end
end

return Calculator