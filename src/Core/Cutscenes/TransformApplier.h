#pragma once

#include "Applier.h"

namespace GuGu {
	class VectorApplier : public Applier
	{
	public:
		VectorApplier();

		virtual ~VectorApplier() override;

		//applier
		virtual void Apply(std::shared_ptr<meta::Object> inObject, const meta::Variant& value, const GuGuUtf8Str& fieldName) override;
	};
}