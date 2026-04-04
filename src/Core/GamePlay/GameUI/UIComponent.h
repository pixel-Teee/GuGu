#pragma once

#include <Core/GamePlay/Component.h>
#include <Core/Math/MyMath.h>

#include "UIPointerData.h" //ui pointer data

#include <Core/GamePlay/Delegate/GuGuScriptDelegate.h>

namespace GuGu {
	struct UIDrawInfo;
	struct UIDebugInfo;
	class UIComponent : public Component
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS

		UIComponent();

		virtual ~UIComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		virtual std::shared_ptr<UIDrawInfo> generateUIDrawInformation(bool bFlip = false);

		virtual std::shared_ptr<UIDebugInfo> generateUIDebugInfomartion(float inLineWidth = 10.0f);

		virtual math::float2 getDesiredSize() const;

		virtual void onPointerDown(UIPointerData pointerData);

		virtual void onPointerUp(UIPointerData pointerData);

		std::shared_ptr<GuGuScriptDelegate> getPointerDownScriptDelegate();
	
		void setPointerDownScriptDelegate(std::shared_ptr<GuGuScriptDelegate> inScriptDelegate);

		std::shared_ptr<GuGuScriptDelegate> getPointerUpScriptDelegate();
	
		void setPointerUpScriptDelegate(std::shared_ptr<GuGuScriptDelegate> inScriptDelegate);

		/*
			可用的基本委托
		*/
		std::shared_ptr<GuGuScriptDelegate> m_onPointerDown;

		std::shared_ptr<GuGuScriptDelegate> m_onPointerUp;
	};
}