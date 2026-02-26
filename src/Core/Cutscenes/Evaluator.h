#pragma once

namespace GuGu {
	struct TrackData;
	class Evaluator
	{
	public:
		virtual ~Evaluator() = default;

		virtual meta::Variant Evaluate(const TrackData& inData, float time) = 0;
	};
}