#include <pch.h>

#include "HeaderRow.h"
#include <Core/UI/Splitter.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/CompoundWidget.h>
#include <Core/UI/ComboButton.h>
#include <Core/UI/Overlay.h>
#include <Core/UI/NullWidget.h>
#include <Core/UI/Box.h>

namespace GuGu {
    class TableColumnHeader : public CompoundWidget
    {
    public:
		struct BuilderArguments : public Arguments<TableColumnHeader>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;

			ARGUMENT_VALUE(std::shared_ptr<TableColumnHeaderStyle>, Style)
		};

        TableColumnHeader()
        : m_sortMode(ColumnSortMode::None)
        , m_sortPriority(ColumnSortPriority::Primary)
        , m_onSortModeChanged()
        , m_contextMenuContent(NullWidget::getNullWidget())
        , m_columnId()
        , m_style(nullptr)
        {}

        void init(const TableColumnHeader::BuilderArguments& inArgs, const HeaderRow::GColumn& column, 
        const Padding defaultHeaderContentPadding)
        {
            m_style = inArgs.mStyle;
            m_columnId = column.m_columnId;
            m_sortMode = column.m_sortMode;
            m_sortPriority = column.m_sortPriority;

            m_onSortModeChanged = column.m_onSortModeChanged;
            m_contextMenuContent = column.m_headerContent;

            m_comboVisibility = column.m_headerComboVisibility;

            Padding adjustedDefaultHeaderContentPadding = defaultHeaderContentPadding;

            Attribute<GuGuUtf8Str> labelText = column.m_defaultText;
            Attribute<GuGuUtf8Str> tooltipText = column.m_defaultTooltip;

            if (column.m_headerContent == NullWidget::getNullWidget())
            {
                //todo:处理这里的内容
            }

            std::shared_ptr<HorizontalBox> box;
            std::shared_ptr<Overlay> overlay = WIDGET_NEW(Overlay);

            overlay->addSlot(0)
			.setChildWidget
			(
				WIDGET_ASSIGN_NEW(HorizontalBox, box)
			);

            std::shared_ptr<Widget> primaryContent = column.m_headerContent;
            if (primaryContent == NullWidget::nullWidget)
            {
                primaryContent = WIDGET_NEW(BoxWidget)
                .padding(m_onSortModeChanged ? Padding(0, 2, 0, 2) : Padding(0, 4, 0, 4))
                .VAlign(VerticalAlignment::Center)
                .Content
                (
                    WIDGET_NEW(TextBlockWidget)
                    .text(labelText)//todo:add tooltip
                );
            };

            box->addSlot()
            .StretchWidth(1.0f)
			(
				primaryContent
			);
        }
    private:
        //现在的排序模式
        Attribute<ColumnSortMode::Type> m_sortMode;
        //现在的排序顺序
        Attribute<ColumnSortPriority::Type> m_sortPriority;

        OnSortModeChanged m_onSortModeChanged;

        std::shared_ptr<Widget> m_contextMenuContent;

        std::shared_ptr<ComboButton> m_comboButton;

        //combo button 的可见性
        HeaderComboVisibility m_comboVisibility;

        std::shared_ptr<Overlay> m_menuOverlay;

        GuGuUtf8Str m_columnId;

        std::shared_ptr<TableColumnHeaderStyle> m_style;
    };

    HeaderRow::HeaderRow()
    {
    }
    HeaderRow::~HeaderRow()
    {
    }

	void HeaderRow::init(const BuilderArguments& inArgs)
	{
        m_scrollBarThickness = math::float2(0, 0);
        m_scrollBarVisibility = Visibility::Collapsed;
        m_style = inArgs.mStyle;
        m_resizeMode = inArgs.mresizeMode;

        Border::init(Border::BuilderArguments()
            .padding(0)
            .brush(m_style->m_backgroundBrush)
        );

        bool bHaveFillerColumn = false;
		for (int32_t slotIndex = 0; slotIndex < inArgs.m_slots.size(); ++slotIndex)
		{
			std::shared_ptr<GColumn> column = inArgs.m_slots[slotIndex];
			m_columns.push_back(column);
		}
	}

	void HeaderRow::regenerateWidgets()
	{
        const float splitterHandleDetectionSize = 5.0f;

        std::shared_ptr<Splitter> splitter;

        //std::shared_ptr<HorizontalBox> box = 
	}

}