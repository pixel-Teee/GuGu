#pragma once

#include <Core/GuGuUtf8Str.h>

#include "json.hpp"

namespace GuGu {
	namespace meta {
		class Type;
		class ArrayWrapper;
		class VariantBase
		{
		public:
			virtual ~VariantBase(void) {}

			virtual Type GetType(void) const = 0;

			virtual void* GetPtr(void) const = 0;

			virtual int ToInt(void) const = 0;
			virtual bool ToBool(void) const = 0;
			virtual float ToFloat(void) const = 0;
			virtual double ToDouble(void) const = 0;
			virtual GuGuUtf8Str ToString(void) const = 0;

			virtual bool IsArray() const;
			virtual ArrayWrapper GetArray(void) const;

			virtual VariantBase* Clone(void) const = 0;

			//todo:add on serialize and on deserialize
			virtual void OnSerialize(nlohmann::json& output) const {}
			virtual void OnDeserialize(const nlohmann::json& input) {}
		};
	}
}