#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	class GAnimation;
	class GStaticMesh;
	struct GMeshGeometry;
	struct AssetData;
	//animator
	class GAnimator : public meta::Object
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		GAnimator();

		virtual ~GAnimator();

		virtual void PostLoad() override;

		virtual void Update(float fElapsedTimeSeconds) override;
		virtual meta::Type GetType() const override;
		//拷贝对象
		virtual Object* Clone(void) const override;
		//序列化
		virtual void OnSerialize(nlohmann::json& output) const override;
		//反序列化
		virtual void OnDeserialize(const nlohmann::json& input) override;

		//play function
		void playAnimation(std::shared_ptr<GAnimation> anim, std::shared_ptr<GStaticMesh> inStaticMesh);

		void playAnimationWithAsset(std::shared_ptr<AssetData> inAnim, std::shared_ptr<AssetData> inStaticMesh);

		void stopAnimation();

		void calculateBoneTransform(const GMeshGeometry& inCurrentNode, math::float4x4 parentTransform);

		bool isRunning() const;
		
		Array<math::float4x4> m_finalBoneMatrices;

		float m_currentTime = 0.0f;

		std::weak_ptr<GAnimation> m_anim;
		std::weak_ptr<GStaticMesh> m_skeletonMesh;

		//runtime
		bool m_isRunning = false;
	};
}