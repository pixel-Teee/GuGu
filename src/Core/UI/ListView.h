#pragma once

#include "TableViewBase.h" //table view base

namespace GuGu {
	template<typename ItemType>
	class ListView : public TableViewBase
	{
	public:

		struct BuilderArguments : public Arguments<ListView>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			
		};

		void init(const BuilderArguments& arguments)
		{
			//制作table view
			
		}
	};
}