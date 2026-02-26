#include <pch.h>

#include "Evaluator.h"
#include "TransformEvaluator.h"
#include <Core/Math/MyMath.h>

namespace GuGu {

	std::shared_ptr<GuGu::Evaluator> createEvaluator(const GuGuUtf8Str& typeGuid)
	{
		meta::Type currentType = meta::Type::getType(typeGuid);
		if (currentType == typeof(math::float3))
		{
			return std::make_shared<VectorEvaluator>();
		}
		else if (currentType == typeof(math::double3))
		{
			return std::make_shared<VectorEvaluator>();
		}
		return nullptr;
	}

}