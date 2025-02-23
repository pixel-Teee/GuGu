#include <pch.h>

#include "HeaderRow.h"
#include <Core/UI/Splitter.h>
#include <Core/UI/BoxPanel.h>
#include <Core/UI/CompoundWidget.h>
#include <Core/UI/ComboButton.h>
#include <Core/UI/Overlay.h>
#include <Core/UI/NullWidget.h>
#include <Core/UI/Box.h>
#include <Core/UI/Spacer.h>
#include <Core/UI/ImageWidget.h>
#include <Core/UI/Border.h>

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

            if (column.m_headerContent == NullWidget::getNullWidget())//header content 没提供，就会使用一个默认的文本
            {
                if (!column.m_defaultText.IsSet())
                {
                    labelText = GuGuUtf8Str(column.m_columnId + "[LabelMissing]");
                }

                if (!column.m_defaultTooltip.IsSet())
                {
                    tooltipText = labelText;
                }
            }

            std::shared_ptr<HorizontalBox> box;
            std::shared_ptr<Overlay> overlay = WIDGET_NEW(Overlay);

            overlay->addSlot(0)
			(
				WIDGET_ASSIGN_NEW(HorizontalBox, box)
			);

            std::shared_ptr<Widget> primaryContent = column.m_headerContent;
            if (primaryContent == NullWidget::getNullWidget())
            {
                primaryContent = WIDGET_NEW(BoxWidget)
                .padding(m_onSortModeChanged ? Padding(0, 2, 0, 2) : Padding(0, 4, 0, 4))
                .HAlign(HorizontalAlignment::Left)
                .VAlign(VerticalAlignment::Center)
                .Content
                (
                    WIDGET_NEW(TextBlockWidget)
                    .text(labelText)//todo:add tooltip
                    .textColor(math::float4(0.19f, 0.16f, 0.13f, 1.0f)) //beige 9
                );
            };

            box->addSlot()
            .StretchWidth(1.0f)
			(
				primaryContent //标题内容
			);

            if (column.m_headerMenuContent != NullWidget::getNullWidget())
            {
                //添加 drop down 按钮，如果 menu content 已经被描述

                box->addSlot()
                .FixedWidth()
                (
                    WIDGET_ASSIGN_NEW(Overlay, m_menuOverlay)
                    .visibility(Attribute<Visibility>::CreateSP(this, &TableColumnHeader::getMenuOverlayVisibility))
                    + Overlay::Slot()
                    (
                        WIDGET_NEW(Spacer)
                        .size(math::float2(12.0f, 0.0f))
                    )
                    + Overlay::Slot()
                    (
                        WIDGET_NEW(Border)
                        .padding(Padding(0.0f, 0.0f, adjustedDefaultHeaderContentPadding.right, 0.0f))
                        .brush(this, &TableColumnHeader::getComboButtonBorderBrush)
                        .Content
                        (
                            WIDGET_ASSIGN_NEW(ComboButton, m_comboButton)
                            .hasDownArrow(false)
                            .buttonStyle(CoreStyle::getStyleSet()->getStyle<ButtonStyle>("NoBorder"))
                            .contentPadding(Padding(0.0))
                            .buttonContent
                            (
                                WIDGET_NEW(Spacer)
                                .size(math::float2(14.0f, 0.0f))
                            )
                            .menuContent
                            (
                                 m_contextMenuContent
                            )
                        )
                    )
                    + Overlay::Slot()
                    .setHorizontalAlignment(HorizontalAlignment::Center)
                    .setVerticalAlignment(VerticalAlignment::Center)
                    .setPadding(Padding(0, 0, 0, 2))
                    (       
                        WIDGET_NEW(ImageWidget)
                        .brush(m_style->m_menuDropDownImage)
                        .visibility(Visibility::HitTestInvisible)   
                    )
                );

                adjustedDefaultHeaderContentPadding.right = 0.0f;
            }

            //HorizontalBox由combo button和一个label组合而成
            //overlay 由这个HorizontalBox和ImageWidget叠在一起，ImageWidget居中，HorizontalBox居左
            //ImageWidget是一个三角形，表示顺序
            overlay->addSlot(1)
			.setHorizontalAlignment(HorizontalAlignment::Center)
			.setVerticalAlignment(VerticalAlignment::Top)
            (
				WIDGET_NEW(ImageWidget)
				.brush(this, &TableColumnHeader::getSortingBrush)
				.visibility(Attribute<Visibility>::CreateSP(this, &TableColumnHeader::getSortModeVisibility))
            );

			m_childWidget = std::make_shared<SingleChildSlot>();
			m_childWidget->m_parentWidget = shared_from_this();
            m_childWidget->m_childWidget = 
            WIDGET_NEW(Border)
            .brush(this, &TableColumnHeader::getHeaderBackgroundBrush)
            .horizontalAlignment(column.m_headerHAlignment)
            .verticalAlignment(column.m_headerVAlignment)
            .padding(column.m_headerContentPadding.value_or(adjustedDefaultHeaderContentPadding))
            .Content
            (
                overlay
            );
            m_childWidget->m_childWidget->setParentWidget(shared_from_this());

            //这个 table column header 一个文本标题右边带了一个 combo button ，可以展开
        }

        ColumnSortMode::Type getSortMode() const
        {
            return m_sortMode.Get();
        }

        std::shared_ptr<Brush> getHeaderBackgroundBrush() const
        {
            if (IsHovered() && m_sortMode.IsBound())
            {
                return m_style->m_hoveredBrush;
            }
            return m_style->m_normalBrush;
        }

        std::shared_ptr<Brush> getSortingBrush() const
        {
            ColumnSortPriority::Type columnSortPriority = m_sortPriority.Get();

            //默认降序
            return (m_sortMode.Get() == ColumnSortMode::Ascending ?
                   (m_sortPriority.Get() == ColumnSortPriority::Secondary ? m_style->m_sortSecondaryAscendingImage : m_style->m_sortPrimaryAscendingImage) :
                   (m_sortPriority.Get() == ColumnSortPriority::Secondary ? m_style->m_sortSecondaryDescendingImage : m_style->m_sortPrimaryDesendingImage));
        }

        Visibility getSortModeVisibility() const
        {
            return (m_sortMode.Get() != ColumnSortMode::None) ? Visibility::HitTestInvisible : Visibility::Hidden;
        }

        //combo button 的可见性性质函数
        Visibility getMenuOverlayVisibility() const 
        {
            if (m_comboVisibility == HeaderComboVisibility::OnHover)
            {
                if (!m_comboButton || !(IsHovered() || m_comboButton->isOpen()))
                {
                    return Visibility::Collapsed;
                }
            }

            return Visibility::Visible;
        }

        std::shared_ptr<Brush> getComboButtonBorderBrush() const
        {
            if (m_comboButton && (m_comboButton->IsHovered() || m_comboButton->isOpen()))
            {
                return m_style->m_menuDropDownHoveredBorderBrush;
            }
            if(IsHovered() || m_comboVisibility == HeaderComboVisibility::Always)
            {
                return m_style->m_menuDropDownNormalBorderBrush;
            }

            return CoreStyle::getStyleSet()->getBrush("NoBorder");
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

        //背景板
        Border::init(Border::BuilderArguments()
            .padding(0)
            .brush(m_style->m_backgroundBrush)
        );

        //拷贝列信息
        bool bHaveFillerColumn = false;
		for (int32_t slotIndex = 0; slotIndex < inArgs.m_slots.size(); ++slotIndex)
		{
			std::shared_ptr<GColumn> column = inArgs.m_slots[slotIndex];
			m_columns.push_back(column);
		}

        //生成针对所有列的控件
        regenerateWidgets();
	}

	const std::vector<std::shared_ptr<HeaderRow::GColumn>>& HeaderRow::getColumns() const
	{
        return m_columns;
	}

	void HeaderRow::clearColumns()
	{
        const bool bHadColumnsItems = m_columns.size() > 0;
        m_columns.clear();
        if (bHadColumnsItems)
        {
            //todo:添加委托
        }

        regenerateWidgets();
	}

	void HeaderRow::addColumn(const GColumn::BuilderArguments& newColumnArgs)
	{
        std::shared_ptr<GColumn> newColumn = std::make_shared<GColumn>(newColumnArgs);
        addColumn(newColumn);
	}

	void HeaderRow::addColumn(std::shared_ptr<GColumn> newColumn)
	{
        int32_t insertIndex = m_columns.size();
        insertColumn(newColumn, insertIndex);
	}

	void HeaderRow::insertColumn(std::shared_ptr<GColumn> newColumn, int32_t insertIndex)
	{
        auto it = m_columns.begin() + insertIndex;

        m_columns.insert(it, newColumn);

        //todo:添加 columns changed 委托
        regenerateWidgets();
	}

	void HeaderRow::regenerateWidgets()
	{
        const float splitterHandleDetectionSize = 5.0f;
        m_headerWidgets.clear();

        std::shared_ptr<Splitter> splitter;

        std::shared_ptr<HorizontalBox> box = 
            WIDGET_NEW(HorizontalBox)
            + HorizontalBox::Slot()
            .StretchWidth(1.0f)
            (
                WIDGET_ASSIGN_NEW(Splitter, splitter)
                .splitterStyle(m_style->m_columnSplitterStyle)
                .resizeMode(m_resizeMode)
                .physicalSplitterHandleSize(0.0f)
                .hitDetectionSplitterHandleSize(splitterHandleDetectionSize) //todo:添加 get row size for slot index
            )
            + HorizontalBox::Slot()
            .FixedWidth()
            .setPadding(0.0f)
            (
                WIDGET_NEW(Spacer)
                .size(m_scrollBarThickness)
                .visibility(m_scrollBarVisibility)
            );

        //构造针对所有列的控件
        {
            const float halfSplitterDetectionSize = (splitterHandleDetectionSize + 2) / 2;

            //填充表示这些列的控件到槽里面
            std::shared_ptr<TableColumnHeader> newlyMadeHeader;
            for (int32_t slotIndex = 0; slotIndex < m_columns.size(); ++slotIndex)
            {
                GColumn& someColumn = *m_columns[slotIndex];
                if (someColumn.m_shouldGenerateWidget.Get(true))
                {
                    //追踪上一个我们构造的 header
                    std::shared_ptr<TableColumnHeader> precedingHeader = newlyMadeHeader;
                    newlyMadeHeader.reset();

                    //left is half splitter detection size
                    //right is half splitter detection size
                    Padding defaultPadding = Padding(halfSplitterDetectionSize, 0, halfSplitterDetectionSize, 0);

                    std::shared_ptr<TableColumnHeader> newHeader = 
                        WIDGET_ASSIGN_NEW(TableColumnHeader, newlyMadeHeader, someColumn, defaultPadding)
                        .Style((slotIndex + 1 == m_columns.size()) ? m_style->m_lastColumnStyle : m_style->m_columnStyle);

                    m_headerWidgets.push_back(newlyMadeHeader);

                    switch (someColumn.m_sizeRule)
                    {
                        case ColumnSizeMode::Fill:
                        {
                            Attribute<float> widthBinding;
                            widthBinding.bindRaw(&someColumn, &GColumn::getWidth);

                            std::function<void(float)> func = std::bind(&GColumn::setWidth, &someColumn, std::placeholders::_1);

                            //添加可缩放的cell
                            splitter->addSlot()
                            .value(widthBinding)
                            .sizeRule(Splitter::FractionOfParent)
                            .onSlotResized(func)
                             (
                                newHeader
                             );

                            break;
                        }
                        case ColumnSizeMode::Fixed:
                        {
                            //add fixed size cell
                            splitter->addSlot()
                            .sizeRule(Splitter::SizeToContent)
                            (
                                WIDGET_NEW(BoxWidget)
                                .WidthOverride(someColumn.getWidth())
                                .Content
                                (
                                    newHeader
                                )
                            );
                            break;
                        }
                        case ColumnSizeMode::Manual:
                        {
                            //把缩放 grip 放在列的最后，不使用 splitter ，是因为他没有我们需要的缩放行为
                            const float gripSize = 5.0f;
                            std::shared_ptr<Border> sizingGrip = WIDGET_NEW(Border)
                            .padding(0.0f)
                            .brush(CoreStyle::getStyleSet()->getBrush("NoBorder"))
                            .Content
                            (
                                WIDGET_NEW(Spacer)
                                .size(math::float2(gripSize, gripSize))
                            );//todo:添加 cursor 响应

                            std::weak_ptr<Border> weakSizingGrip = sizingGrip;
                            auto sizingGrip_OnMouseButtonDown = [&someColumn, weakSizingGrip](const WidgetGeometry&, const PointerEvent&)->Reply
                            {
                                std::shared_ptr<Border> sizingGripPtr = weakSizingGrip.lock();
                                if(sizingGripPtr)
                                {
                                    return Reply::Handled().captureMouse(sizingGripPtr);
                                }
                                return Reply::Unhandled();
                            };

							auto sizingGrip_OnMouseButtonUp = [&someColumn, weakSizingGrip](const WidgetGeometry&, const PointerEvent&)->Reply
							{
								std::shared_ptr<Border> sizingGripPtr = weakSizingGrip.lock();
								if (sizingGripPtr && sizingGripPtr->hasMouseCapture())
								{
									return Reply::Handled().releaseMouseCapture();
								}
								return Reply::Unhandled();
							};

							auto sizingGrip_OnMouseMove = [&someColumn, weakSizingGrip](const WidgetGeometry&, const PointerEvent& inPointerEvent)->Reply
							{
								std::shared_ptr<Border> sizingGripPtr = weakSizingGrip.lock();
								if (sizingGripPtr && sizingGripPtr->hasMouseCapture())
								{
                                    const float newWith = someColumn.getWidth() + inPointerEvent.getCursorDelta().x;
                                    someColumn.setWidth(std::max(20.0f, newWith));
									return Reply::Handled();
								}
								return Reply::Unhandled();
							};

                            sizingGrip->setOnMouseButtonDown(sizingGrip_OnMouseButtonDown);
                            sizingGrip->setOnMouseButtonUp(sizingGrip_OnMouseButtonUp);
                            sizingGrip->setOnMouseMove(sizingGrip_OnMouseMove);

                            //这个函数创建 optional size ，值为 desired width
                            auto getColumnWidthAsOptionalSize = [&someColumn]()->OptionalSize
                            {
                               const float desiredWidth = someColumn.getWidth();
                               return OptionalSize(desiredWidth);
                            };

							Attribute<OptionalSize> widthBinding;
							widthBinding.bind(getColumnWidthAsOptionalSize);

                            //add resizeable cell
                            splitter->addSlot()
                            .sizeRule(Splitter::SizeToContent)
                            (
                                WIDGET_NEW(BoxWidget)
                                .WidthOverride(widthBinding)
                                .Content
                                (
                                    WIDGET_NEW(Overlay)
                                    + Overlay::Slot()
                                    (
                                        newHeader
                                    )
                                    + Overlay::Slot()
                                    .setHorizontalAlignment(HorizontalAlignment::Right)
                                    (
                                        sizingGrip
                                    )
                                )
                            );

                            break;
                        }
                    }
                }
            }
        }

        //把 box 放到 border 上
        setContent(box);
	}

}