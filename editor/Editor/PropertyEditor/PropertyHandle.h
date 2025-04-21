#pragma once

namespace GuGu {
	namespace PropertyAccess
	{
		enum Result
		{
			NultipleValue,
			Fail,
			Success
		};
	}
	class IPropertyHandle : public std::enable_shared_from_this<IPropertyHandle>
	{
	public:
		virtual ~IPropertyHandle() {}

		virtual PropertyAccess::Result setValue(const float& inValue) = 0;
		virtual PropertyAccess::Result getValue(float& outValue) const = 0;
	};
}