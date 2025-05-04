#pragma once

#include "IPropertyTypeCustomization.h"
#include <vector>
#include <optional>
#include <memory>
#include <Core/UI/BasicElement.h>

namespace GuGu {
	class Widget;
	class IPropertyHandle;
	class MathStructCustomization : public IPropertyTypeCustomization
	{
	public:
		virtual void customizeHeader(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow);

		virtual void getSortedChildren(std::shared_ptr<IPropertyHandle> structPropertyHandle, std::vector<std::shared_ptr<IPropertyHandle>>& outChildren);

		virtual std::shared_ptr<Widget> makeChildWidget(std::shared_ptr<IPropertyHandle>& structPropertyHandle, std::shared_ptr<IPropertyHandle>& propertyHandle);
	private:
		template<typename NumericType>
		std::shared_ptr<Widget> makeNumericWidget(std::shared_ptr<IPropertyHandle>& structPropertyHandle, std::shared_ptr<IPropertyHandle>& propertyHandle);

		template<typename NumericType>
		void onValueCommitted(NumericType newValue, TextCommit::Type commitType, std::weak_ptr<IPropertyHandle> weakHandlePtr);

		template<typename NumericType>
		void onValueChanged(NumericType newValue, std::weak_ptr<IPropertyHandle> weakHandlePtr);

		template<typename NumericType>
		std::optional<NumericType> onGetValue(std::weak_ptr<IPropertyHandle> weakHandlePtr) const;

		template<typename NumericType>
		void setValue(NumericType newValue, std::weak_ptr<IPropertyHandle> weakHandlePtr);
	protected:
		virtual void makeHeaderRow(std::shared_ptr<IPropertyHandle> propertyHandle, DetailWidgetRow& headerRow);

		std::vector<std::shared_ptr<IPropertyHandle>> m_sortedChildHandles;
	};

	


}