local class = require("content/Script/Common/Class")
local Calculator = class()

function Calculator:init(owner)
	self.owner = owner

	self.textComponent = self.owner:getChildren("Text_FPS"):getComponent("GuGu::TextComponent")

	self.frameCount = 0
	self.elapsedTime = 0

	local currentLevel = self.owner:getCurrentLevel()
	self.centerBorder = self.owner:getCurrentLevel():getGameObject("CenterBorder")
	if currentLevel then
		local newGameObject = currentLevel:createGameObject("screent_1")
		newGameObject:addComponentFromName("GuGu::UITransformComponent");
		if newGameObject then
			self.centerBorder:addChildren(newGameObject)
			local textComponent = newGameObject:addComponentFromName("GuGu::TextComponent");
			textComponent:setText("114514")
			print("add text component successful")
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