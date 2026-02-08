#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include "PropertyCustomizationHelpers.h"

namespace GuGu {
	class PropertyMenuGameObjectPicker : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<PropertyMenuGameObjectPicker>
		{
			BuilderArguments() {};

			~BuilderArguments() = default;

			UI_EVENT(OnGameObjectSelected, onGameObjectSelected)
		};
		PropertyMenuGameObjectPicker();

		virtual ~PropertyMenuGameObjectPicker();

		void init(const BuilderArguments& arguments);

	private:

	};
}