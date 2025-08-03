#pragma once

#include <Core/Reflection/Type.h>
#include <Core/Reflection/Object.h>

#include <Core/Math/MyMath.h> //math

namespace GuGu {

	class UIPointerData : public meta::Object {
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS

		UIPointerData();

		virtual ~UIPointerData();

		virtual meta::Type GetType() const override;

		virtual void PostLoad() {}

		virtual void Update(float fElapsedTimeSeconds);

		virtual Object* Clone(void) const override;

		virtual void OnSerialize(nlohmann::json& output) const {}
		virtual void OnDeserialize(const nlohmann::json& input) {}

		//screen position
		math::float2 getScreenPosition() const;
		math::float2& getScreenPosition();
		void setScreenPosition(const math::float2& inScreenPosition);
	private:
		math::float2 m_screenPosition;
	};
}