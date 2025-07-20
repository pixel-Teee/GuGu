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

		UIPadding getUIOffset() const;
		UIPadding& getUIOffset();
		void setUIOffset(UIPadding inUIPadding);

		UIAnchors getUIAnchors() const;
		UIAnchors& getUIAnchors();
		void setUIAnchors(UIAnchors inUIAnchors);

		math::float2 getAlignment() const;
		math::float2& getAlignment();
		void setAlignment(math::float2 inAlignment);

		bool getAutoSize() const;
		bool& getAutoSize();
		void setAutoSize(bool autoSize);

		float getZOrder() const;
		float& getZOrder();
		void setZOrder(float zOrder);

		math::float2 getLocalSize() const;
		math::float2& getLocalSize();
		void setLocalSize(math::float2 inLocalSize);

		math::float2 getLocalPosition() const;
		math::float2& getLocalPosition();
		void setLocalPosition(math::float2 inLocalPosition);
	private:
		void calculateLayout();

		UIAnchors m_anchors; //anchor

		UIPadding m_offset; //offset

		math::float2 m_alignment;

		bool m_autoSize;

		float m_zOrder;

		//local size(calculated)
		math::float2 m_localSize;

		//local position(calculated)
		math::float2 m_localPosition;
	};
}