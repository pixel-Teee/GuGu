#pragma once

#include "Widget.h"
#include "IScrollableWidget.h"
#include "BasicElement.h"
#include "HeaderRow.h"
#include "ScrollBar.h"
#include "Style.h"
#include "ITypedTableView.h"

namespace GuGu {
	using OnTableViewScrolled = std::function<void(double)>;//从 item 列表开始的 scroll offset

	enum class ListItemAlignment : uint8_t
	{
		//item 沿着线均匀地分布(在item之间添加任何额外的空间作为填充)
		EvenlyDistributed,

		//item 沿着线均匀地分布(任何额外的空间都用于按比例放大item的大小)
		EvenlySize,

		//item 沿着线均匀地分布(任何额外的空间都用于按比例放大item的宽度)
		EvenlyWide,

		LeftAligned,

		RightAligned,

		CenterAligned,

		Fill
	};

	//当计算table layout elements 的时候，抽象出X和Y的需求
	struct TableViewDimensions
	{
		TableViewDimensions(Orientation inOrientation);
		TableViewDimensions(Orientation inOrientation, float x, float y);
		TableViewDimensions(Orientation inOrientation, const math::float2& size);

		math::float2 toVector2d() const
		{
			return m_orientation == Orientation::Vertical ? math::float2(m_lineAxis, m_scrollAxis) : math::float2(m_scrollAxis, m_lineAxis);
		}

		TableViewDimensions operator+(const TableViewDimensions& other) const
		{
			return TableViewDimensions(m_orientation, toVector2d() + other.toVector2d());
		}

		Orientation m_orientation = Orientation::Vertical;

		//沿着 table view 滑动轴的维度 (当垂直的时候是Y，当水平的时候是X)
		float m_scrollAxis = 0.0f;
		//垂直于滑动轴的维度，沿着被创建的 items 的行，只对 tile views 有用
		float m_lineAxis = 0.0f;
	};

	class ITableRow;
	class ListPanel;
	class TableViewBase : public Widget, public IScrollableWidget
	{
	public:
		//创建构成list的child widgets
		void constructChildren(const Attribute<float>& inItemWidth,
			const Attribute<float>& inItemHeight,
			const Attribute< ListItemAlignment>& inItemAlignment,
			const std::shared_ptr<HeaderRow>& inHeaderRow,
			const std::shared_ptr<ScrollBar>& inScrollBar,
			Orientation inScrollOrientation,
			const OnTableViewScrolled& inOnTableViewScrolled,
			const Attribute<std::shared_ptr<ScrollBarStyle>> inScrollBarStyle);

		virtual int32_t getNumItemsBeingObserved() const = 0;

		//当用户滑动的时候，通过 scroll bar 调用
		//这个是被 scroll bar 调用的
		void scrollBarOnUserScrolled(float inScrollOffsetFraction);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;

		virtual void Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime) override;

		//获取可以沿着 line axis 塞入的items 数量，默认是1，TileView 会有很多
		virtual int32_t getNumItemsPerLine() const;

		enum class ScrollIntoViewResult
		{
			//滑入一个 item 到 view
			Success,

			//没有足够的数据去滑入给定的 item 到 view ，延迟到下一次 tick
			Deferred,

			//滑入确切的 item 失败
			Failure
		};

		virtual ScrollIntoViewResult scrollIntoView(const WidgetGeometry& listViewGeometry) = 0;

		virtual float getNumLiveWidgets() const;

		//这里是否有一个刷新请求
		bool isPendingRefresh() const;

		//设置这个 view 的 scroll offset (在 items)
		void setScrollOffset(const float inScrollOffset);

		//内部的请求 ，对于一个布局的更新，在下一次 tick
		void requestLayoutRefresh();

		double getTargetScrollOffset() const;

		struct ReGenerateResults
		{
			ReGenerateResults(double inNewScrollOffset, double inLengthGenerated, double inItemsOnScreen,
				bool atEndOfList)
				: m_newScrollOffset(inNewScrollOffset)
				, m_lengthOfGeneratedItems(inLengthGenerated)
				, m_exactNumLinesOnScreen(inItemsOnScreen)
				, m_bGeneratePastLastItem(atEndOfList)
			{}

			//我们实际使用的用户不渴望的 scroll offset
			double m_newScrollOffset = 0;

			//沿着 scroll axis 的 widgets 的总的 length ，去表示 items 的子集
			double m_lengthOfGeneratedItems = 0;

			//多少行适合塞进 screen ，包括 fractions
			double m_exactNumLinesOnScreen = 0;

			//当我们已经生成完毕后，为真
			bool m_bGeneratePastLastItem = false;
		};

		virtual ReGenerateResults reGenerateItems(const WidgetGeometry& myGeometry) = 0;

		void clearWidgets();

		void appendWidget(const std::shared_ptr<ITableRow>& widgetToAppend);

		void insertWidget(const std::shared_ptr<ITableRow>& widgetToAppend);

		void requestListRefresh();

		virtual void rebuildList() = 0;

		const TableViewMode::Type m_tableViewMode;
	protected:
		TableViewBase(TableViewMode::Type inTableViewMode);

		//列的头，描述这个 list 展示的列
		std::shared_ptr<HeaderRow> m_headerRow;

		//当 table view 被滑动的时候，调用的委托
		OnTableViewScrolled m_onTableViewScrolled;

		//list 的布局还有 scroll 的方向
		Orientation m_orientation = Orientation::Vertical;

		//持有 list 中可见的 widgets 的 panel
		std::shared_ptr<ListPanel> m_itemsPanel;

		//scroll bar widget
		std::shared_ptr<ScrollBar> m_scrollBar;

		std::shared_ptr<SingleChildSlot> m_childWidget;

		//上一次刷新发生的时候，list 的 geometry
		WidgetGeometry m_panelGeometryLastTick;

		//items 的列表开始的 scroll offset
		double m_currentScrollOffset = 0.0f;

		double m_desiredScrollOffset = 0.0f;

		//上一次重新生成 pass 的信息
		ReGenerateResults m_lastGenerateResults;

		bool b_wasAtEndOfList;
	private:
		bool m_bItemsNeedRefresh = false;
		
	};
}