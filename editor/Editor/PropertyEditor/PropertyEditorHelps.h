#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>

namespace GuGu {
	class PropertyEditor;
	class PropertyNameWidget : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<PropertyNameWidget>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments, std::shared_ptr<PropertyEditor> inPropertyEditor);

	private:
		std::shared_ptr<PropertyEditor> m_propertyEditor;
	};

}