#pragma once

#include <Core/GamePlay/TransformComponent.h>

#include "UIAnchors.h"
#include "UIPadding.h"

namespace GuGu {
	class UITransformComponent : public TransformComponent
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS

		UITransformComponent();

		virtual ~UITransformComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		UIPadding getUIPadding() const;
		UIPadding& getUIPadding();
		void setUIPadding(UIPadding inUIPadding);

		UIAnchors getUIAnchors() const;
		UIAnchors& getUIAnchors();
		void setUIAnchors(UIAnchors inUIAnchors);
	private:
		void calculateLayout();

		UIAnchors m_anchors; //anchor

		UIPadding m_offset; //offset

		math::float2 m_alignment;

		bool m_autoSize;

		float m_zOrder;
	};
}