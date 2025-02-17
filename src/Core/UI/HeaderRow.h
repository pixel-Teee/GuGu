#pragma once

#include "Border.h"
#include "Splitter.h" //splitter

namespace GuGu {
	class HeaderRow : public Border
	{
	public:
		class FColumn
		{
		public:
			struct BuilderArguments : public Arguments<FColumn>
			{
				BuilderArguments()
				{}

				~BuilderArguments() = default;


			};
		};

		HeaderRow();

		virtual ~HeaderRow();

		using ColumnsChanged = std::function<void(const std::shared_ptr<HeaderRow>&)>;

		struct BuilderArguments : public Arguments<HeaderRow>
		{
			BuilderArguments()
			{}

			~BuilderArguments() = default;

			ARGUMENT_VALUE(std::shared_ptr<HeaderRowStyle>, Style)

			ARGUMENT_VALUE(SplitterResizeMode::Type, resizeMode)

			UI_EVENT(ColumnsChanged, onColumnsChanged)
		};

		void init(const BuilderArguments& inArgs);

	private:
		std::shared_ptr<HeaderRowStyle> m_style;
		SplitterResizeMode::Type m_resizeMode;
		math::float2 m_scrollBarThickness;
		Attribute<Visibility> m_scrollBarVisibility;
	};
}