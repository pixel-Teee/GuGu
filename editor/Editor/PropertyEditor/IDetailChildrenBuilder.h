#pragma once

namespace GuGu {
	class IDetailPropertyRow;
	class IPropertyHandle;
	class IDetailChildrenBuilder
	{
	public:
		virtual ~IDetailChildrenBuilder() {}

		virtual IDetailPropertyRow& addProperty(std::shared_ptr<IPropertyHandle> propertyHandle) = 0;
	};
}