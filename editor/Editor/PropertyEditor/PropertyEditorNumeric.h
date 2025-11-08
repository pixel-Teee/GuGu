#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include <Core/UI/NumericEntryBox.h>
#include <Core/Reflection/Field.h>
#include <Core/Reflection/MetaProperty/Range.h>

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

			//get range
			NumericType minValue = std::numeric_limits<NumericType>::min();
			NumericType maxValue = std::numeric_limits<NumericType>::max();
			
			std::shared_ptr<IPropertyHandle> propertyHandle = inPropertyEditor->getPropertyHandle();
			if (propertyHandle)
			{
				const meta::Field* field = propertyHandle->getField();
				if (field)
				{
					const meta::Range* range = field->GetMeta().GetProperty<meta::Range>();
					if (range)
					{
						minValue = range->m_minValue;
						maxValue = range->m_maxValue;
					}
				}
			}

			const std::shared_ptr<PropertyNode> propertyNode = inPropertyEditor->getPropertyNode();
			const meta::Field* field = inPropertyEditor->getField();
			m_childWidget = std::make_shared<SingleChildSlot>();
			m_childWidget->m_parentWidget = shared_from_this();
			m_childWidget->m_childWidget =
			WIDGET_ASSIGN_NEW(NumericEntryBox<NumericType>, m_primaryWidget)
			.allowSpain(true) //todo:修复这个
			.value(Attribute<std::optional<NumericType>>(this, &PropertyEditorNumeric<NumericType>::onGetValue)) //要显示的值
			.onValueChanged(this, &PropertyEditorNumeric<NumericType>::onValueChanged)
			.onValueCommitted(this, &PropertyEditorNumeric<NumericType>::onValueCommitted)
			.minValue(minValue)
			.maxValue(maxValue);
			m_childWidget->m_childWidget->setParentWidget(shared_from_this());
		}

		void getFixedWidth(float& outMinFixedWidth, float& outMaxFixedWidth)
		{
			const meta::Field* field = m_propertyEditor->getField();

			outMinFixedWidth = 125.0f;
			outMaxFixedWidth = 125.0f;
		}

		static bool supports(const std::shared_ptr<PropertyEditor>& inPropertyEditor)
		{
			const std::shared_ptr<PropertyNode> propertyNode = inPropertyEditor->getPropertyNode();
			return TTypeToProperty<NumericType>::match(inPropertyEditor->getField());
		}

		std::optional<NumericType> onGetValue() const
		{
			NumericType numericVal;

			const std::shared_ptr<IPropertyHandle> propertyHandle = m_propertyEditor->getPropertyHandle();
			if(propertyHandle)
				propertyHandle->getValue(numericVal);

			return numericVal;
		}

		void onValueChanged(NumericType newValue)
		{
			const std::shared_ptr<IPropertyHandle> propertyHandle = m_propertyEditor->getPropertyHandle();

			NumericType orgValue(0);
			if (propertyHandle->getValue(orgValue) != PropertyAccess::Fail)
			{
				if (orgValue == newValue)
					return;
			}

			propertyHandle->setValue(newValue);
		}

		void onValueCommitted(NumericType newValue, TextCommit::Type commitInfo)
		{
			const std::shared_ptr<IPropertyHandle> propertyHandle = m_propertyEditor->getPropertyHandle();

			NumericType orgValue(0);
			if (propertyHandle->getValue(orgValue) != PropertyAccess::Fail)
			{
				if (orgValue == newValue)
					return;
			}

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
			static bool match(const meta::Field* inField) { return inField->GetType() == typeof(float); }
		};
		template<typename U>
		struct TTypeToProperty<double, U>
		{
			static bool match(const meta::Field* inField) { return inField->GetType() == typeof(double); }
		};

		template<typename U>
		struct TTypeToProperty<int8_t, U>
		{
			static bool match(const meta::Field* inField) { return inField->GetType() == typeof(int8_t); }
		};

		template<typename U>
		struct TTypeToProperty<int16_t, U>
		{
			static bool match(const meta::Field* inField) { return inField->GetType() == typeof(int16_t); }
		};

		template<typename U>
		struct TTypeToProperty<int32_t, U>
		{
			static bool match(const meta::Field* inField) { return inField->GetType() == typeof(int32_t); }
		};

		template<typename U>
		struct TTypeToProperty<int64_t, U>
		{
			static bool match(const meta::Field* inField) { return inField->GetType() == typeof(int64_t); }
		};

		template<typename U>
		struct TTypeToProperty<uint8_t, U>
		{
			static bool match(const meta::Field* inField) { return inField->GetType() == typeof(uint8_t); }
		};

		template<typename U>
		struct TTypeToProperty<uint16_t, U>
		{
			static bool match(const meta::Field* inField) { return inField->GetType() == typeof(uint16_t); }
		};

		template<typename U>
		struct TTypeToProperty<uint32_t, U>
		{
			static bool match(const meta::Field* inField) { return inField->GetType() == typeof(uint32_t); }
		};

		template<typename U>
		struct TTypeToProperty<uint64_t, U>
		{
			static bool match(const meta::Field* inField) { return inField->GetType() == typeof(uint64_t); }
		};
		std::shared_ptr<PropertyEditor> m_propertyEditor;

		std::shared_ptr<Widget> m_primaryWidget;
	};
}