#include <pch.h>

#include "TableViewBase.h"
#include "ListPanel.h"
#include "BoxPanel.h"
#include "Box.h"//box widget
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"
#include "LayoutUtils.h"
#include "ITableRow.h"
#include "WidgetPath.h"

#include <Application/Application.h>

namespace GuGu {
	TableViewDimensions::TableViewDimensions(Orientation inOrientation)
        : m_orientation(inOrientation)
	{
	}
	TableViewDimensions::TableViewDimensions(Orientation inOrientation, float x, float y)
        : TableViewDimensions(inOrientation, math::float2(x, y))
	{
	}
	TableViewDimensions::TableViewDimensions(Orientation inOrientation, const math::float2& size)
        : TableViewDimensions(inOrientation)
	{
        if (inOrientation == Orientation::Vertical)
        {
            m_lineAxis = size.x;
            m_scrollAxis = size.y;
        }
        else
        {
			m_lineAxis = size.y;
			m_scrollAxis = size.x;
        }
	}
    void TableViewBase::constructChildren(const Attribute<float>& inItemWidth, 
        const Attribute<float>& inItemHeight, 
        const Attribute<ListItemAlignment>& inItemAlignment, 
        const std::shared_ptr<HeaderRow>& inHeaderRow, 
        const std::shared_ptr<ScrollBar>& inScrollBar, 
        Orientation inScrollOrientation, 
        const OnTableViewScrolled& inOnTableViewScrolled, 
        const Attribute<std::shared_ptr<ScrollBarStyle>> inScrollBarStyle)
    {
        m_bItemsNeedRefresh = true;

        m_headerRow = inHeaderRow;

        m_onTableViewScrolled = inOnTableViewScrolled;

        //如果有 header row，那么就是垂直的，否则根据参数提供的方向来提供
        m_orientation = inHeaderRow ? Orientation::Vertical : inScrollOrientation;

        m_itemsPanel = WIDGET_NEW(ListPanel)
            .Clip(getClipping())
            .ItemWidth(inItemWidth)
            .ItemHeight(inItemHeight)
            .NumDesiredItems(this, &TableViewBase::getNumItemsBeingObserved)
            .ItemAlignment(inItemAlignment.Get())
            .ListOrientation(m_orientation);

        std::shared_ptr<Widget> listAndScrollbar;
        if (inScrollBar) //external scroll bar
        {
            //用户提供的 scroll bar
            m_scrollBar = inScrollBar;

            m_scrollBar->setOnUserScrolled(std::bind(&TableViewBase::scrollBarOnUserScrolled, this, std::placeholders::_1));

            listAndScrollbar = m_itemsPanel;
        }
        else
        {
            m_scrollBar = WIDGET_NEW(ScrollBar)
                .onUserScrolled(this, &TableViewBase::scrollBarOnUserScrolled)
                .orientation(m_orientation)
                .style(inScrollBarStyle.IsSet() ? inScrollBarStyle : CoreStyle::getStyleSet()->template getStyle<ScrollBarStyle>("ScrollBar"));

            const OptionalSize scrollBarSize(16.0f);

            if (m_orientation == Orientation::Vertical)
            {
                listAndScrollbar = WIDGET_NEW(HorizontalBox)
                    + HorizontalBox::Slot()
                    .StretchWidth(1.0f)
                    (
                        m_itemsPanel
                    )
                    +HorizontalBox::Slot()
                    .FixedWidth()
                    (
                        WIDGET_NEW(BoxWidget)
                        .WidthOverride(scrollBarSize)
                        .Content
                        (
                            m_scrollBar
                        )
                   );
            }
            else
            {
                listAndScrollbar = WIDGET_NEW(VerticalBox)
                    + VerticalBox::Slot()
                    .StretchHeight(1.0f)
                    (
                        m_itemsPanel
                    )
                    + VerticalBox::Slot()
                    .FixedHeight()
                    (
                        WIDGET_NEW(BoxWidget)
                        .HeightOverride(scrollBarSize)
                        .Content
                        (
                            m_scrollBar
                        )
                    );
            }
        }

        this->m_childWidget = std::make_shared<SingleChildSlot>();

        if (inHeaderRow)
        {
            //todo ：我们需要关联 scroll bar, 如果我们创建了它

			this->m_childWidget->setChildWidget
			(
				WIDGET_NEW(VerticalBox)
                + VerticalBox::Slot()
                .FixedHeight()
                (
                    inHeaderRow
                )
                + VerticalBox::Slot()
                .StretchHeight(1)
                (
                    listAndScrollbar
                )
			);			
        }
        else
        {
            this->m_childWidget->setChildWidget
            (
                listAndScrollbar
            );
        }
		this->m_childWidget->m_parentWidget = shared_from_this();
        this->m_childWidget->m_childWidget->setParentWidget(shared_from_this());
    }

    void TableViewBase::scrollBarOnUserScrolled(float inScrollOffsetFraction)
    {
        //滑动的 items 数量
        const double clampedScrollOffsetInItems = std::clamp(inScrollOffsetFraction, 0.0f, 1.0f) * getNumItemsBeingObserved();

        scrollTo(clampedScrollOffsetInItems);
    }

    uint32_t TableViewBase::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
    {
		ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);//设置数组只接受可见的child widget
		AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

		uint32_t widgetNumbers = arrangedWidgetArray.getArrangedWidgetsNumber();//note:just one
		//math::double2 size = math::double2(0.0, 0.0);
		uint32_t maxLayer = 0;
		for (size_t i = 0; i < widgetNumbers; ++i)
		{
			std::shared_ptr<ArrangedWidget> childWidget = arrangedWidgetArray.getArrangedWidget(i);
			if (childWidget)
			{
				std::shared_ptr<Widget> widget = childWidget->getWidget();

				maxLayer = std::max(maxLayer, widget->generateElement(paintArgs, cullingRect, elementList, childWidget->getWidgetGeometry(), layer + 1));
			}
		}

		return maxLayer;
    }

    math::float2 TableViewBase::ComputeFixedSize(float inLayoutScaleMultiplier)
    {
		if (m_childWidget)
		{
			const Visibility childVisiblity = m_childWidget->getChildWidget()->getVisibility();
			if (childVisiblity != Visibility::Collapsed)
			{
				return m_childWidget->getChildWidget()->getFixedSize() + m_childWidget->getPadding().getFixedSize();
			}
		}
		return math::float2(0, 0);
    }

    void TableViewBase::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
    {
		//arrange single children
		uint32_t slotNumber = getSlotsNumber();
		//check this is one
		//assert(slotNumber == 1);

		if (slotNumber)
		{
			const Visibility childVisibility = getSlot(0)->getChildWidget()->getVisibility();

			if (arrangedWidgetArray.accepts(childVisibility)) //数组的可见性是否接受widget的可见性
			{
				AlignmentArrangeResult xalignmentResult = AlignChild<Orientation::Horizontal>(*getSlot(0), allocatedGeometry.getLocalSize().x);
				AlignmentArrangeResult yAlignmentResult = AlignChild<Orientation::Vertical>(*getSlot(0), allocatedGeometry.getLocalSize().y);

				WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(math::float2(xalignmentResult.m_size, yAlignmentResult.m_size), math::float2(xalignmentResult.m_offset, yAlignmentResult.m_offset));

				arrangedWidgetArray.pushWidget(childGeometry, getSlot(0)->getChildWidget());
			}
		}
    }

    SlotBase* TableViewBase::getSlot(uint32_t index) const
    {
        return m_childWidget.get();
    }

    uint32_t TableViewBase::getSlotsNumber() const
    {
        return 1;
    }

    void TableViewBase::Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime)
    {
        if (m_itemsPanel)
        {
            WidgetGeometry panelGeometry = findChildGeometry(allocatedGeometry, m_itemsPanel);

            if (m_bItemsNeedRefresh || m_panelGeometryLastTick.getLocalSize().x != panelGeometry.getLocalSize().x || m_panelGeometryLastTick.getLocalSize().y != panelGeometry.getLocalSize().y)
            {
                m_panelGeometryLastTick = panelGeometry;

                const int32_t numItemsPerLine = getNumItemsPerLine();
                //可能存在将某个 item 滚进 panel 的请求，先处理这个
                const ScrollIntoViewResult scrollInToViewResult = scrollIntoView(panelGeometry);

                double targetScrollOffset = getTargetScrollOffset();

                m_currentScrollOffset = targetScrollOffset;

                const ReGenerateResults reGenerateResults = reGenerateItems(panelGeometry);
                m_lastGenerateResults = reGenerateResults;

                const int32_t numItemsBeingObserved = getNumItemsBeingObserved();
                const int32_t numItemsLines = numItemsBeingObserved / numItemsPerLine;

                const double initialDesiredOffset = m_desiredScrollOffset;
                const bool bEnoughRoomForAllItems = reGenerateResults.m_exactNumLinesOnScreen >= numItemsLines;//有足够的空间显示所有控件

                if (bEnoughRoomForAllItems)
                {
                    setScrollOffset(0.0f);
                    m_currentScrollOffset = targetScrollOffset = m_desiredScrollOffset;
                }
                else if (reGenerateResults.m_bGeneratePastLastItem)
                {
                    setScrollOffset(std::max(0.0, reGenerateResults.m_newScrollOffset));
                    m_currentScrollOffset = targetScrollOffset = m_desiredScrollOffset;
                }

                //set first line scroll offset
                //这个很重要，设置起始的偏移
                m_itemsPanel->setFirstLineScrollOffset(getFirstLineScrollOffset());

                //update scroll bar
                //更新 scroll bar
                if (numItemsBeingObserved > 0)
                {
                    if (reGenerateResults.m_exactNumLinesOnScreen < 1.0f)
                    {
                        const double visibleSizeFraction = allocatedGeometry.getLocalSize().y / reGenerateResults.m_lengthOfGeneratedItems;
                        const double thumbSizeFraction = std::min(visibleSizeFraction, 1.0);
                        const double offsetFraction = m_currentScrollOffset / numItemsBeingObserved;
                        //设置 thumb 的起始位置 和 thumb 大小 占整个 scroll bar 的百分比
                        m_scrollBar->setState(offsetFraction, thumbSizeFraction);
                    }
                    else
                    {
                        const double thumbSizeFraction = reGenerateResults.m_exactNumLinesOnScreen / numItemsLines;
						const double offsetFraction = m_currentScrollOffset / numItemsBeingObserved;
						m_scrollBar->setState(offsetFraction, thumbSizeFraction);
                    }
                }
                else
				{
                    const double thumbSizeFraction = 1;
                    const double offsetFraction = 0;
					m_scrollBar->setState(offsetFraction, thumbSizeFraction);
                }

                b_wasAtEndOfList = (m_scrollBar->distanceFromBottom() < (1.e-8f));

                m_bItemsNeedRefresh = false;
                m_itemsPanel->setRefreshPending(false);
            }
        }
    }

    Reply TableViewBase::OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
        if (inMouseEvent.m_effectingButton == Keys::RightMouseButton)
        {
            onRightMouseButtonUp(inMouseEvent);

            Reply reply = Reply::Handled().releaseMouseCapture();

            return reply;
        }

        return Reply::Unhandled();
    }

    Reply TableViewBase::OnMouseButtonDoubleClick(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
        if (this->hasMouseCapture())
        {
            return Reply::Handled();
        }
        return Reply::Unhandled();
    }

    Reply TableViewBase::OnMouseWheel(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent)
    {
        if (!inMouseEvent.isControlDown())
        {
            const float additionalOffset = (inMouseEvent.m_wheelOrGestureDelta >= 0.0f ? -1.0f : 1.0f) * getNumItemsPerLine();
            const double newScrollOffset = std::max(0.0, m_desiredScrollOffset + additionalOffset);
            this->scrollTo(newScrollOffset);
            return Reply::Handled();
        }
        return Reply::Unhandled();
    }

    int32_t TableViewBase::getNumItemsPerLine() const
    {
        return 1;
    }

    float TableViewBase::getNumLiveWidgets() const
    {
        return m_itemsPanel->getSlotsNumber();
    }

    void TableViewBase::onRightMouseButtonUp(const PointerEvent& mouseEvent)
    {
        const math::float2& summonLocation = mouseEvent.m_screenSpacePosition;
        const bool bContextMenuOpeningBound = m_onContextMenuOpening.operator bool();
        if (bContextMenuOpeningBound)
        {
            std::shared_ptr<Widget> menuContent = m_onContextMenuOpening();

            if (menuContent)
            {
                WidgetPath widgetPath = WidgetPath();
                Application::getApplication()->pushMenu(shared_from_this(), widgetPath, menuContent, summonLocation);
            }
        }
    }

    bool TableViewBase::isPendingRefresh() const
    {
        return m_bItemsNeedRefresh || m_itemsPanel->isRefreshPending();
    }

    void TableViewBase::setScrollOffset(const float inScrollOffset)
    {
        const float inValidatedOffset = std::max(0.0f, inScrollOffset);
        if (m_desiredScrollOffset != inValidatedOffset)
        {
            m_desiredScrollOffset = inValidatedOffset;
            if (m_onTableViewScrolled)
            {
               m_onTableViewScrolled(m_desiredScrollOffset);
            }
            requestLayoutRefresh();
        }
    }

    void TableViewBase::requestLayoutRefresh()
    {
        if (!m_bItemsNeedRefresh)
        {
            m_bItemsNeedRefresh = true;
        }

        if (m_itemsPanel)
        {
            m_itemsPanel->setRefreshPending(true);
        }
    }

    double TableViewBase::getTargetScrollOffset() const
    {
        return m_desiredScrollOffset;
    }

    void TableViewBase::clearWidgets()
    {
        m_itemsPanel->clearItems();
    }

    float TableViewBase::getItemWidth() const
    {
        return getItemSize().x;
    }

    float TableViewBase::getItemHeight() const
    {
        return getItemSize().y;
    }

    math::float2 TableViewBase::getItemSize() const
    {
        TableViewDimensions itemDimensions = m_itemsPanel->getItemSize(m_panelGeometryLastTick);
        itemDimensions.m_lineAxis += m_itemsPanel->getItemPadding(m_panelGeometryLastTick);
        return itemDimensions.toVector2d();
    }

    void TableViewBase::appendWidget(const std::shared_ptr<ITableRow>& widgetToAppend)
    {
        m_itemsPanel->addSlot()
        (
            widgetToAppend->asWidget()
        );
    }

    void TableViewBase::insertWidget(const std::shared_ptr<ITableRow>& widgetToAppend)
    {
		m_itemsPanel->addSlot(0)
		(
			widgetToAppend->asWidget()
		);
    }

    void TableViewBase::requestListRefresh()
    {
        requestLayoutRefresh();
    }

    float TableViewBase::scrollTo(float inScrollOffset)
    {
        const float newScrollOffset = std::clamp(inScrollOffset, -10.0f, getNumItemsBeingObserved() + 10.0f);
        float amountScrolled = std::abs(m_desiredScrollOffset - newScrollOffset);

        setScrollOffset(newScrollOffset);

        if (b_wasAtEndOfList && newScrollOffset >= m_desiredScrollOffset)
        {
            amountScrolled = 0;
        }

        return amountScrolled;
    }

    float TableViewBase::getFirstLineScrollOffset() const
    {
        const double firstLineScrollOffset = m_currentScrollOffset / getNumItemsPerLine();
        return firstLineScrollOffset - (int64_t)firstLineScrollOffset;
    }

    TableViewBase::TableViewBase(TableViewMode::Type inTableViewMode)
        : m_tableViewMode(inTableViewMode)
        , m_lastGenerateResults(0, 0, 0, false)
    {
    }

}