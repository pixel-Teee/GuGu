#include <pch.h>

#include "ListPanel.h"
#include "ArrangedWidget.h"
#include "ArrangedWidgetArray.h"

namespace GuGu {
    ListPanel::ListPanel()
    {
    }
    ListPanel::~ListPanel()
    {
    }
    void ListPanel::init(const BuilderArguments& arguments)
    {
        m_itemWidth = arguments.mItemWidth;
        m_itemHeight = arguments.mItemHeight;
        m_numDesiredItems = arguments.mNumDesiredItems;
        m_itemAlignment = arguments.mItemAlignment;
        m_orientation = arguments.mListOrientation;

		m_childrens.resize(arguments.mSlots.size());
		for (size_t i = 0; i < arguments.mSlots.size(); ++i)
		{
			std::shared_ptr<ListPanel::PaneSlot> slot = std::static_pointer_cast<ListPanel::PaneSlot>(arguments.mSlots[i].m_slot);
            PaneSlot::SlotBuilderArguments builderArguments = arguments.mSlots[i];
			//slot->init(shared_from_this(), builderArguments);
			//m_childrens[i] = std::reinterpret_pointer_cast<BoxPanelSlot>(boxSlot);
			m_childrens[i] = std::static_pointer_cast<PaneSlot>(std::static_pointer_cast<SlotBase>(slot));
		}
		m_widgetClipping = arguments.mClip;
		m_visibilityAttribute = arguments.mVisibility;

        //m_overScrollAmount = 40.0f;
    }
    math::float2 ListPanel::ComputeFixedSize(float inLayoutScaleMultiplier)
    {
        TableViewDimensions desiredListPanelDimensions(m_orientation);

        if (shouldArrangeAsTiles())
        {
            TableViewDimensions itemDimensions = getDesiredItemDimensions();//单个 item 的宽度和高度

            desiredListPanelDimensions.m_lineAxis = itemDimensions.m_lineAxis;
            desiredListPanelDimensions.m_scrollAxis = itemDimensions.m_scrollAxis;
        }
        else if (m_childrens.size() > 0)
        {
            for (int32_t itemIndex = 0; itemIndex < m_childrens.size(); ++itemIndex)
            {
                TableViewDimensions childDimensions(m_orientation, m_childrens[itemIndex]->getChildWidget()->getFixedSize());
                desiredListPanelDimensions.m_scrollAxis += childDimensions.m_scrollAxis;//滑动轴的长度
                desiredListPanelDimensions.m_lineAxis = std::max(desiredListPanelDimensions.m_lineAxis, childDimensions.m_lineAxis);
            }

            desiredListPanelDimensions.m_scrollAxis /= m_childrens.size();
        }

        desiredListPanelDimensions.m_scrollAxis *= m_preferredNumLines; //item 数量
        return desiredListPanelDimensions.toVector2d();
    }
    void ListPanel::AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const
    {
        if (m_childrens.size() > 0)
        {
            const TableViewDimensions allottedDimensions(m_orientation, allocatedGeometry.getLocalSize());
            TableViewDimensions dimensionsSoFar(m_orientation);

            if (shouldArrangeAsTiles())
            {
                //这是一个tile view list - 安排 items 逐个沿着 line axis 直到这里没有空间，那么就创建一个新的行沿着 scroll axis
                const ListItemAlignment listItemAlignment = m_itemAlignment.Get();
                const float itemPadding = getItemPadding(allocatedGeometry, listItemAlignment);
                const float halfItemPadding = itemPadding * 0.5f;

                const TableViewDimensions localItemSize = getItemSize(allocatedGeometry, listItemAlignment);
                dimensionsSoFar.m_scrollAxis = -std::floor(m_firstLineScrollOffset * localItemSize.m_scrollAxis) - m_overScrollAmount;

                bool isNewLine = true;
                for (int32_t itemIndex = 0; itemIndex < m_childrens.size(); ++itemIndex)
                {
                    if (isNewLine)
                    {
                        if (listItemAlignment == ListItemAlignment::RightAligned || listItemAlignment == ListItemAlignment::CenterAligned)
                        {
                            const float linePadding = getLinePadding(allocatedGeometry, itemIndex);
                            const bool isVisible = m_childrens[itemIndex]->getChildWidget()->getVisibility().isVisible();
                            if (listItemAlignment == ListItemAlignment::RightAligned)
                            {
                                dimensionsSoFar.m_lineAxis += isVisible ? linePadding : 0.0f;
                            }
                            else
                            {
                                const float halfLinePadding = linePadding * 0.5f;
                                dimensionsSoFar.m_lineAxis += isVisible ? halfLinePadding : 0.0f;
                            }
                        }

                        isNewLine = false;
                    }

                    TableViewDimensions childOffset = dimensionsSoFar;
                    childOffset.m_lineAxis += halfItemPadding;
					WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(localItemSize.toVector2d(), childOffset.toVector2d());

					arrangedWidgetArray.pushWidget(childGeometry, m_childrens[itemIndex]->getChildWidget());
                    
                    dimensionsSoFar.m_lineAxis += localItemSize.m_lineAxis + itemPadding;
                    if (dimensionsSoFar.m_lineAxis + localItemSize.m_lineAxis + itemPadding > allottedDimensions.m_lineAxis) //一行行摆放，如果超出区域，就起新行
                    {
                        dimensionsSoFar.m_lineAxis = 0;
                        dimensionsSoFar.m_scrollAxis += localItemSize.m_scrollAxis;
                        isNewLine = true;
                    }
                }
            }
            else
            {
                //这是一个普通的 list ，安排 items 沿着 scroll axis 在一行
                const TableViewDimensions firstChildDimensions(m_orientation, m_childrens[0]->getChildWidget()->getFixedSize());

                //over scroll amount 是移动的数量
                dimensionsSoFar.m_scrollAxis = -std::floor(m_firstLineScrollOffset * firstChildDimensions.m_scrollAxis) - m_overScrollAmount;

                for (int32_t itemIndex = 0; itemIndex < m_childrens.size(); ++itemIndex)
                {
                    const TableViewDimensions widgetDesiredDimensions(m_orientation, m_childrens[itemIndex]->getChildWidget()->getFixedSize());
                    const bool bIsVisible = m_childrens[itemIndex]->getChildWidget()->getVisibility().isVisible();

                    TableViewDimensions finalWidgetDimensions(m_orientation);
                    finalWidgetDimensions.m_scrollAxis = bIsVisible ? widgetDesiredDimensions.m_scrollAxis : 0.0f;
                    finalWidgetDimensions.m_lineAxis = allottedDimensions.m_lineAxis;

					WidgetGeometry childGeometry = allocatedGeometry.getChildGeometry(finalWidgetDimensions.toVector2d(), dimensionsSoFar.toVector2d());

					arrangedWidgetArray.pushWidget(childGeometry, m_childrens[itemIndex]->getChildWidget());

                    dimensionsSoFar.m_scrollAxis += finalWidgetDimensions.m_scrollAxis;
                }
            }
        }
    }
    uint32_t ListPanel::onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer)
    {
		uint32_t maxLayerId = layer;

		ArrangedWidgetArray arrangedWidgetArray(Visibility::Visible);
		AllocationChildActualSpace(allocatedGeometry, arrangedWidgetArray);

		uint32_t widgetsNumber = arrangedWidgetArray.getArrangedWidgetsNumber();
		for (uint32_t childIndex = 0; childIndex < widgetsNumber; ++childIndex)
		{
			std::shared_ptr<ArrangedWidget> childWidget = arrangedWidgetArray.getArrangedWidget(childIndex);
			if (childWidget)
			{
				std::shared_ptr<Widget> widget = childWidget->getWidget();

				const uint32_t curWidgetsMaxLayerId = childWidget->getWidget()->generateElement(paintArgs, cullingRect, elementList, childWidget->getWidgetGeometry(), layer);
				maxLayerId = std::max(maxLayerId, curWidgetsMaxLayerId);
			}
		}

		return maxLayerId;
    }
    void ListPanel::Tick(const WidgetGeometry& allottedGeometry, const double inCurrentTime, const float inDeltaTime)
    {
        if (shouldArrangeAsTiles())
        {
			const TableViewDimensions allottedDimensions(m_orientation, allottedGeometry.getLocalSize());

			const ListItemAlignment listItemAlignment = m_itemAlignment.Get();
			const float itemPadding = getItemPadding(allottedGeometry, listItemAlignment);
			TableViewDimensions itemDimensions = getItemSize(allottedGeometry, listItemAlignment);
			itemDimensions.m_lineAxis += itemPadding;

			const int32_t NumChildren = m_numDesiredItems.Get();

			if (itemDimensions.m_lineAxis > 0.0f && NumChildren > 0)
			{
				const int32_t NumItemsPerLine = std::clamp((int32_t)std::ceilf(allottedDimensions.m_lineAxis / itemDimensions.m_lineAxis) - 1, 1, NumChildren);
                m_preferredNumLines = std::ceilf(NumChildren / (float)NumItemsPerLine);
			}
			else
			{
                m_preferredNumLines = 1;
			}
        }
        else
        {
            m_preferredNumLines = m_numDesiredItems.Get();
        }
    }
    SlotBase* ListPanel::getSlot(uint32_t index) const
    {
        return m_childrens[index].get();
    }
    uint32_t ListPanel::getSlotsNumber() const
    {
        return m_childrens.size();
    }
    TableViewDimensions ListPanel::getDesiredItemDimensions() const
    {
        return TableViewDimensions(m_orientation, m_itemWidth.Get(), m_itemHeight.Get());
    }
    float ListPanel::getItemPadding(const WidgetGeometry& allottedGeometry, const ListItemAlignment listItemAlignment) const
    {
        if (listItemAlignment == ListItemAlignment::EvenlyDistributed)
        {
            TableViewDimensions allottedDimensions(m_orientation, allottedGeometry.getLocalSize());
            TableViewDimensions desiredDimensions = getDesiredItemDimensions();

            const int32_t numItemsPerLine = desiredDimensions.m_lineAxis > 0.0f ? std::floor(allottedDimensions.m_lineAxis / desiredDimensions.m_lineAxis) : 0.0f;
            if (numItemsPerLine > 0.0f && m_childrens.size() > numItemsPerLine)
            {
                return (allottedDimensions.m_lineAxis - numItemsPerLine * desiredDimensions.m_lineAxis) / numItemsPerLine;
            }
        }

        return 0.0f;
    }
    TableViewDimensions ListPanel::getItemSize(const WidgetGeometry& allottedGeometry) const
    {
        return getItemSize(allottedGeometry, m_itemAlignment.Get());
    }
    TableViewDimensions ListPanel::getItemSize(const WidgetGeometry& allottedGeometry, const ListItemAlignment listItemAlignment) const
    {
        const TableViewDimensions allottedDimensions(m_orientation, allottedGeometry.getLocalSize());
        const TableViewDimensions itemDimensions = getDesiredItemDimensions();

        //每行的个数
        const int32_t numItemsPerLine = itemDimensions.m_lineAxis > 0.0f ? std::floor(allottedDimensions.m_lineAxis / itemDimensions.m_lineAxis) : 0.0f;

        TableViewDimensions extraDimensions(m_orientation);

        if (numItemsPerLine)
        {
            if (listItemAlignment == ListItemAlignment::Fill ||
                listItemAlignment == ListItemAlignment::EvenlyWide ||
                listItemAlignment == ListItemAlignment::EvenlySize)
            {
                //获取额外的宽高 extra dimensions
                extraDimensions.m_lineAxis = (allottedDimensions.m_lineAxis - numItemsPerLine * itemDimensions.m_lineAxis) / numItemsPerLine;

                if (listItemAlignment == ListItemAlignment::EvenlySize)
                {
                    extraDimensions.m_scrollAxis = itemDimensions.m_scrollAxis * (extraDimensions.m_lineAxis / (itemDimensions.m_lineAxis + extraDimensions.m_lineAxis));
                }
            }
        }

        return itemDimensions + extraDimensions;
    }
    float ListPanel::getLinePadding(const WidgetGeometry& allottedGeometry, const int32_t lineStartIndex) const
    {
        const int32_t numItemsLeft = m_childrens.size() - lineStartIndex;
        if (numItemsLeft <= 0.0f)
        {
            return 0.0f;
        }

        const TableViewDimensions allottedDimensions(m_orientation, allottedGeometry.getLocalSize());
        const TableViewDimensions itemSize = getItemSize(allottedGeometry);

        const int32_t maxNumItemsOnLine = itemSize.m_lineAxis > 0.0f ? std::floor(allottedDimensions.m_lineAxis / itemSize.m_lineAxis) : 0.0f;
        const int32_t numItemsOnLine = std::min(numItemsLeft, maxNumItemsOnLine);

        return allottedDimensions.m_lineAxis - (numItemsOnLine * itemSize.m_lineAxis);
    }
    ListPanel::PaneSlot::SlotBuilderArguments ListPanel::Slot()
    {
		return PaneSlot::SlotBuilderArguments(std::make_shared<PaneSlot>());
    }
    ListPanel::PaneSlot::SlotBuilderArguments ListPanel::addSlot(int32_t insertAtIndex)
    {
        std::shared_ptr<PaneSlot> slot = std::make_shared<PaneSlot>();
        if (insertAtIndex == -1)
            insertAtIndex = m_childrens.size();
        auto it = m_childrens.begin() + insertAtIndex;
        m_childrens.insert(it, slot);//在指定位置插入 slot
        return PaneSlot::SlotBuilderArguments(slot);
    }
    void ListPanel::setRefreshPending(bool isPendingRefresh)
    {
        m_bIsRefreshPending = isPendingRefresh;
    }
    bool ListPanel::isRefreshPending() const
    {
        return m_bIsRefreshPending;
    }
    void ListPanel::clearItems()
    {
        m_childrens.clear();
    }
    bool ListPanel::shouldArrangeAsTiles() const
    {
        TableViewDimensions desiredItemDimensions = getDesiredItemDimensions();
        return desiredItemDimensions.m_lineAxis > 0 && desiredItemDimensions.m_scrollAxis > 0;
    }
}