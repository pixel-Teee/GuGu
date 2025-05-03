#pragma once

namespace GuGu {
	class IDetailLayoutBuilder : public std::enable_shared_from_this<IDetailLayoutBuilder>
	{
	public:
		virtual ~IDetailLayoutBuilder(){}

	};
}