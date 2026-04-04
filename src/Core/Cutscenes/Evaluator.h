#pragma once

namespace GuGu {
	struct TrackData;
	class Evaluator
	{
	public:
		virtual ~Evaluator() = default;

		virtual meta::Variant Evaluate(const TrackData& inData, float time) = 0;

		virtual GuGuUtf8Str getCurrentField() = 0;
	};

	//create evaluator
	std::shared_ptr<Evaluator> createEvaluator(const GuGuUtf8Str& typeGuid);
}