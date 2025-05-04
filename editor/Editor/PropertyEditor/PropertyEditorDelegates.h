#pragma once

#include <functional>
#include <memory>

namespace GuGu {
	class IPropertyTypeCustomization;
	using OnGetPropertyTypeCustomizationInstance = std::function<std::shared_ptr<IPropertyTypeCustomization>()>;
}