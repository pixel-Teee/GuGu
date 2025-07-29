#include <pch.h>

#include "MathStructCustomization.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include "IDetailChildrenBuilder.h"

#include <Core/UI/NumericEntryBox.h>
#include <Core/UI/TextBlockWidget.h>
#include <Core/UI/BoxPanel.h>
#include <Core/Reflection/Field.h>
#include <Core/UI/UIMacros.h>

namespace GuGu {

	void MathStructCustomization::customizeHeader(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow)
	{
		m_sortedChildHandles.clear();//四个分量
		getSortedChildren(propertyHandle, m_sortedChildHandles);
		makeHeaderRow(propertyHandle, headerRow);
	}

	void MathStructCustomization::getSortedChildren(std::shared_ptr<IPropertyHandle> structPropertyHandle, std::vector<std::shared_ptr<IPropertyHandle>>& outChildren)
	{
		uint32_t numChildren;
		structPropertyHandle->getNumChildren(numChildren);

		for (uint32_t childIndex = 0; childIndex < numChildren; ++childIndex)
		{
			outChildren.push_back(structPropertyHandle->getChildHandle(childIndex));
		}
	}

	std::shared_ptr<Widget> MathStructCustomization::makeChildWidget(std::shared_ptr<IPropertyHandle>& structPropertyHandle, std::shared_ptr<IPropertyHandle>& propertyHandle)
	{
		const meta::Field* propertyType = propertyHandle->getField();

		if (propertyType->GetType() == typeof(float))
		{
			return makeNumericWidget<float>(structPropertyHandle, propertyHandle);
		}
		else if (propertyType->GetType() == typeof(double))
		{
			return makeNumericWidget<double>(structPropertyHandle, propertyHandle);
		}

		return NullWidget::getNullWidget();
	}

	void MathStructCustomization::cutomizeChildren(std::shared_ptr<IPropertyHandle> propertyHandle, IDetailChildrenBuilder& childBuilder)
	{
		for (int32_t childIndex = 0; childIndex < m_sortedChildHandles.size(); ++childIndex)
		{
			std::shared_ptr<IPropertyHandle> childHandle = m_sortedChildHandles[childIndex];

			childBuilder.addProperty(childHandle);
		}
	}

	template<typename NumericType>
	std::optional<NumericType> MathStructCustomization::onGetValue(std::weak_ptr<IPropertyHandle> weakHandlePtr) const
	{
		NumericType numericVal = 0;
		if (weakHandlePtr.lock())
		{
			std::shared_ptr<IPropertyHandle> handleLocked = weakHandlePtr.lock();
			if (handleLocked->getValue(numericVal) == PropertyAccess::Success)
			{
				return std::optional<NumericType>(numericVal);
			}
		}
		return std::optional<NumericType>();
	}

	template<typename NumericType>
	void MathStructCustomization::onValueChanged(NumericType newValue, std::weak_ptr<IPropertyHandle> weakHandlePtr)
	{
		setValue(newValue, weakHandlePtr);
	}

	template<typename NumericType>
	void MathStructCustomization::onValueCommitted(NumericType newValue, TextCommit::Type commitType, std::weak_ptr<IPropertyHandle> weakHandlePtr)
	{
		setValue(newValue, weakHandlePtr);
	}

	template<typename NumericType>
	void MathStructCustomization::setValue(NumericType newValue, std::weak_ptr<IPropertyHandle> weakHandlePtr)
	{
		NumericType orginValue;
		if (weakHandlePtr.lock())
		{
			weakHandlePtr.lock()->getValue(orginValue);
			if(orginValue != newValue)
				weakHandlePtr.lock()->setValue(newValue);
		}
	}

	template<typename NumericType>
	std::shared_ptr<Widget> MathStructCustomization::makeNumericWidget(std::shared_ptr<IPropertyHandle>& structPropertyHandle, std::shared_ptr<IPropertyHandle>& propertyHandle)
	{
		std::weak_ptr<IPropertyHandle> weakHandlePtr = propertyHandle;

		return
			WIDGET_NEW(NumericEntryBox<NumericType>)
			.allowSpain(true) //todo:修复这个
			.value(this, &MathStructCustomization::onGetValue<NumericType>, weakHandlePtr) //要显示的值
			.onValueChanged(this, &MathStructCustomization::onValueChanged<NumericType>, weakHandlePtr)
			.onValueCommitted(this, &MathStructCustomization::onValueCommitted<NumericType>, weakHandlePtr)
			.minValue(-1000)
			.maxValue(1000);
	}

	void MathStructCustomization::makeHeaderRow(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow)
	{
		std::shared_ptr<HorizontalBox> horizontalBox;

		headerRow.nameContent()
		(
			propertyHandle->createPropertyNameWidget("")
		)
		.valueContent()
		(
			WIDGET_ASSIGN_NEW(HorizontalBox, horizontalBox)
		);

		for (int32_t childIndex = 0; childIndex < m_sortedChildHandles.size(); ++childIndex)
		{
			std::shared_ptr<IPropertyHandle> childHandle = m_sortedChildHandles[childIndex];

			const bool bLastChild = (m_sortedChildHandles.size() - 1) == childIndex;
			std::shared_ptr<Widget> numericEntryBox = makeChildWidget(propertyHandle, childHandle);

			horizontalBox->addSlot()
			.setHorizontalAlignment(HorizontalAlignment::Center)
			.setVerticalAlignment(VerticalAlignment::Center)
			.FixedWidth()
			.setPadding(Padding(0.0f, 2.0f, bLastChild ? 0.0f : 3.0f, 2.0f))
			(
				numericEntryBox
			);
		}
	}

}