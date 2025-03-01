#pragma once

#include "ObjectMode.h"

namespace GuGu {
	class ObjectModeInteractive : public ObjectMode
	{
	public:
		ObjectModeInteractive(const ObjectModeParams& params);
		virtual ~ObjectModeInteractive();

		virtual bool isInteractive() const override { return true; }

		void onLevelChanged();
	};
}