#pragma once

namespace GuGu {
	class Component {
	public:
		virtual ~Component() {}

		virtual void Update(float fElapsedTimeSeconds) {}
	};
}