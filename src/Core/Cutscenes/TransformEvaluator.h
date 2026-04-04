#pragma once

#include "Evaluator.h"

namespace GuGu {
	//scale 
	//translation
	//double3
	class VectorEvaluator : public Evaluator
	{
	public:
		VectorEvaluator();

		virtual ~VectorEvaluator();

		meta::Variant Evaluate(const TrackData& inData, float time) override;

		virtual GuGuUtf8Str getCurrentField() override;

		GuGuUtf8Str m_currentFieldName;
	};
}