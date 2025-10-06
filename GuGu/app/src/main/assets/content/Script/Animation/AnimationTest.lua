local class = require("content/Script/Common/Class")
local AnimationTest = class()

function AnimationTest:init(owner)
	self.owner = owner
    self.animator = GuGu.GAnimator() --create animator
    self.animation = self.owner:getWorld():loadAnimation("content/Defeated_anim.json")
    self.staticMesh = self.owner:getComponent("GuGu::StaticMeshComponent")
    --get mesh
    self.animator:playAnimationWithAsset(self.animation, self.staticMesh:getStaticMeshAsset())
    local currentLevel = self.owner:getCurrentLevel()
    if currentLevel then
        self.playButton = currentLevel:getGameObject("PlayButton")
        local buttonComponent = self.playButton:getComponent("GuGu::ButtonComponent")
		if buttonComponent then
			buttonComponent.m_onClicked:addFunction(self.owner:getComponent("GuGu::ScriptComponent"), "OnPlayButtonClick")
		end 
        local textComponent = currentLevel:getGameObject("PlayButton_Text"):getComponent("GuGu::TextComponent")
        if textComponent then
			textComponent:setText("Stop")
            self.textComponent = textComponent
		end 
    end
end

function AnimationTest:update(delta)
    self.animator:Update(delta)
end

function AnimationTest:OnPlayButtonClick()
    if self.animator:isRunning() then
        self.animator:stopAnimation()
        self.textComponent:setText("Play")
    else
        self.animator:playAnimationWithAsset(self.animation, self.staticMesh:getStaticMeshAsset())
        self.textComponent:setText("Stop")
    end
end

return AnimationTest