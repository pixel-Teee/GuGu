#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include <Core/UI/NumericEntryBox.h>
#include <Core/Reflection/Field.h>

#include "PropertyEditor.h"
#include "PropertyHandle.h"

namespace GuGu {
	template<typename NumericType>
	class PropertyEditorNumeric : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<PropertyEditorNumeric>
		{
			BuilderArguments() {};

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments, std::shared_ptr<PropertyEditor> inPropertyEditor)
		{
			m_propertyEditor = inPropertyEditor;

			const std::shared_ptr<PropertyNode> propertyNode = inPropertyEditor->getPropertyNode();
			const meta::Field* field = inPropertyEditor->getField();
		}

		void getFixedWidth(float& outMinFixedWidth, float& outMaxFixedWidth)
		{
			const meta::Field* field = m_propertyEditor->getField();

			outMinFixedWidth = 125.0f;
			outMaxFixedWidth = 125.0f;

			m_childWidget = std::make_shared<SingleChildSlot>();
			m_childWidget->m_parentWidget = shared_from_this();
			m_childWidget->m_childWidget = 
			WIDGET_ASSIGN_NEW(NumericEntryBox<NumericType>, m_primaryWidget)
			.allowSpain(true) //todo:修复这个
			.value(Attribute<std::optional<NumericType>>(this, &PropertyEditorNumeric<NumericType>::onGetValue)) //要显示的值
			.onValueChanged(this, &PropertyEditorNumeric<NumericType>::onValueChanged)
			.onValueCommitted(this, &PropertyEditorNumeric<NumericType>::onValueCommitted);
		}

		static bool supports(const std::shared_ptr<PropertyEditor>& inPropertyEditor)
		{
			const std::shared_ptr<PropertyNode> propertyNode = inPropertyEditor->getPropertyNode();
			return TTypeToProperty<NumericType>::match(inPropertyEditor->getField());
		}

		std::optional<NumericType> onGetValue() const
		{
			NumericType numericVal;

			//const std::shared_ptr<IPropertyHandle> propertyHandle = 

			return std::optional<NumericType>();
		}

		void onValueChanged(NumericType newValue)
		{
			const std::shared_ptr<IPropertyHandle> propertyHandle = m_propertyEditor->getPropertyHandle();

			propertyHandle->setValue(newValue);
		}

		void onValueCommitted(NumericType newValue, TextCommit::Type commitInfo)
		{
			const std::shared_ptr<IPropertyHandle> propertyHandle = m_propertyEditor->getPropertyHandle();

			propertyHandle->setValue(newValue);
		}
	private:
		template<typename T, typename U = void>
		struct TTypeToProperty
		{
			static bool match(const meta::Field* inField) { return false; }
		};
		template<typename U>
		struct TTypeToProperty<float, U>
		{
			static bool match(const meta::Field* inField) { return inField->GetType().IsFloatingPoint(); }//todo:修复这个，要更进一步区分float，double
		};
		std::shared_ptr<PropertyEditor> m_propertyEditor;

		std::shared_ptr<Widget> m_primaryWidget;
	};
}