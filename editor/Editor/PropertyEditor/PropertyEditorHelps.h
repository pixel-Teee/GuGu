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

	class PropertyValueWidget : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<PropertyValueWidget>
		{
			BuilderArguments() {};

			~BuilderArguments() = default;

			ARGUMENT_VALUE(bool, ShowPropertyButtons)
		};

		void init(const BuilderArguments& arguments, std::shared_ptr<PropertyEditor> inPropertyEditor);

		float getMinFixedWidth() const { return m_minFixedWidth; }

		float getMaxFixedWidth() const { return m_maxFixedWidth; }

	private:
		std::shared_ptr<Widget> constructPropertyEditorWidget(std::shared_ptr<PropertyEditor>& inPropertyEditor);
	private:
		std::shared_ptr<Widget> m_valueEditorWidget;

		float m_minFixedWidth;

		float m_maxFixedWidth;
	};

	class IPropertyHandle;
	class PropertyNode;
	namespace PropertyEditorHelps
	{
		std::shared_ptr<IPropertyHandle> getPropertyHandle(std::shared_ptr<PropertyNode> inPropertyNode);
	}
	
}