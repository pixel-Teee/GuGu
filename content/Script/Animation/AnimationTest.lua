local class = require("content/Script/Common/Class")
local AnimationTest = class()

function AnimationTest:init(owner)
	self.owner = owner
    self.animator = GuGu.GAnimator.new() --create animator
    self.animation = self.owner:getWorld():loadAnimation("content/RunningInPlace.json")
    self.staticMesh = self.owner:getComponent("GuGu::StaticMeshComponent")
    --get mesh
    self.animator:playAnimationWithAsset(self.animation, self.staticMesh:getStaticMeshAsset())
end

function AnimationTest:update(delta)
    self.animator:Update(delta)
end

return AnimationTest