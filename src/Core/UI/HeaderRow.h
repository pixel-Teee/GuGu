#pragma once

#include "Border.h"
#include "Splitter.h" //splitter

namespace GuGu {
	class HeaderRow : public Border
	{
	public:
		class GColumn
		{
		public:
			struct BuilderArguments : public Arguments<GColumn>
			{
				BuilderArguments()
				: mColumnId()
				{}

				~BuilderArguments() = default;

				ARGUMENT_VALUE(GuGuUtf8Str, ColumnId)
				
			};

			GColumn(const BuilderArguments& inArgs)
			: m_columnId(inArgs.mColumnId)
			{}

			//这一列的ID
			GuGuUtf8Str m_columnId;
		};

		static GColumn::BuilderArguments Column(const GuGuUtf8Str& inColumnId)
		{
			GColumn::BuilderArguments newArgs;
			newArgs.ColumnId(inColumnId);
			return newArgs;
		}

		HeaderRow();

		virtual ~HeaderRow();

		using ColumnsChanged = std::function<void(const std::shared_ptr<HeaderRow>&)>;

		struct BuilderArguments : public Arguments<HeaderRow>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;

			ARGUMENT_VALUE(std::shared_ptr<HeaderRowStyle>, Style)

			ARGUMENT_VALUE(SplitterResizeMode::Type, resizeMode)

			//ARGUMENT_SLOT(GColumn, columnSlots)
			ARGUMENT_SUPPORTS_SLOT_WITH_ARGS(GColumn)

			UI_EVENT(ColumnsChanged, onColumnsChanged)
		};

		void init(const BuilderArguments& inArgs);

	private:
		std::shared_ptr<HeaderRowStyle> m_style;
		SplitterResizeMode::Type m_resizeMode;
		math::float2 m_scrollBarThickness;
		Attribute<Visibility> m_scrollBarVisibility;

		std::vector<std::shared_ptr<GColumn>> m_columns;
	};
}