#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>

namespace GuGu {
	class DetailTableRowBase;
	class Button;
	class Brush;
	class DetailExpanderArrow : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<DetailExpanderArrow>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments, std::shared_ptr<DetailTableRowBase> detailsRow);

		Visibility getExpanderVisibility() const;

		std::shared_ptr<Brush> getExpanderImage() const;

		Reply onExpanderClicked();
	private:
		std::weak_ptr<DetailTableRowBase> m_row;
		std::shared_ptr<Button> m_expanderArrow;
	};
}