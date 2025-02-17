#pragma once

#include "Border.h"
#include "Splitter.h" //splitter

namespace GuGu {
	enum class HeaderComboVisibility
	{
		Always, //显示 drop down 完全的透明度

		Ghosted,

		OnHover //只有悬浮的时候，才显示 drop down
	};

	namespace ColumnSortMode
	{
		enum Type
		{
			None = 0,

			Ascending = 1,

			Descending = 2
		};
	}

	namespace ColumnSortPriority
	{
		enum Type
		{
			None,
			Primary = 0,
			Secondary = 2,
			Max
		};
	}
	using OnWidthChanged = std::function<void(float)>;
	using OnSortModeChanged = std::function<void(ColumnSortPriority::Type, const GuGuUtf8Str&, ColumnSortMode::Type)>;
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
				ARGUMENT_ATTRIBUTE(GuGuUtf8Str, DefaultLabel)
				ARGUMENT_ATTRIBUTE(GuGuUtf8Str, DefaultTooltip)
				ARGUMENT_ATTRIBUTE(float, FillWidth)
				ARGUMENT_VALUE(std::optional<float>, FixedWidth)
				UI_EVENT(OnWidthChanged, onWidthChanged)

				ARGUMENT_NAMED_SLOT(SingleChildSlot, headerContent)
				ARGUMENT_VALUE(HorizontalAlignment, hAlignHeader)
				ARGUMENT_VALUE(VerticalAlignment, vAlignHeader)
				ARGUMENT_VALUE(std::optional<Padding>, headerContentPadding)
				ARGUMENT_VALUE(HeaderComboVisibility, headerComboVisibility)

				ARGUMENT_NAMED_SLOT(SingleChildSlot, MenuContent)

				ARGUMENT_VALUE(HorizontalAlignment, hAlignCell)
				ARGUMENT_VALUE(VerticalAlignment, vAlignCell)

				ARGUMENT_ATTRIBUTE(ColumnSortMode::Type, sortMode)
				ARGUMENT_ATTRIBUTE(ColumnSortPriority::Type, sortPriority)
				UI_EVENT(OnSortModeChanged, onSort)
			};

			GColumn(const BuilderArguments& inArgs)
			: m_columnId(inArgs.mColumnId)
			{}

			//这一列的ID
			GuGuUtf8Str m_columnId;

			Attribute<GuGuUtf8Str> m_defaultText;

			Attribute<GuGuUtf8Str> m_defaultTooltip;

			Attribute<float> m_width;

			float m_defaultWidth;

			OnWidthChanged m_onWidthChanged;
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