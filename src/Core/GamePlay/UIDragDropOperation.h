#pragma once

#include <Core/Reflection/Type.h>
#include <Core/Reflection/Object.h>

namespace GuGu {

	class UIDragDropOperation : public meta::Object
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS

		UIDragDropOperation();

		virtual ~UIDragDropOperation();

		virtual Object* Clone(void) const override;

		void Update(float fElapsedTimeSeconds) override;

		virtual meta::Type GetType() const override;

        virtual void PostLoad() override;
	};
}