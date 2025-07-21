#pragma once

#include <Core/GamePlay/Component.h>

namespace GuGu {
	class CanvasComponent : public Component
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS

		CanvasComponent();

		virtual ~CanvasComponent();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

		float getScaleFactor() const;
		float& getScaleFactor();
		void setScaleFactor(float inScaleFactor);
	private:
		float m_scaleFactor;
	};
}