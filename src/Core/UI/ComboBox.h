#pragma once

#include "TableRow.h"
#include "ComboButton.h"
#include "TableViewTypeTraits.h"//ListTypeTraits
#include "Box.h"
#include "NullWidget.h"

namespace GuGu {
	using OnComboBoxOpening = std::function<void()>;
	template<typename OptionType>
	class ComboRow : public TableRow<OptionType>
	{
	public:
		struct BuilderArguments : public Arguments<ComboRow<OptionType>>
		{
			BuilderArguments()
				//: mContent()
				: mStyle(StyleSet::getStyle()->template getStyle<TableRowStyle>("comboBox.Row"))
				, mpadding(Padding(0))
			{
				//this->mClip = WidgetClipping::ClipToBounds;
			}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(std::shared_ptr<TableRowStyle>, Style)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)

			ARGUMENT_ATTRIBUTE(Padding, padding)
		};
	public:
		void init(const typename ComboRow<OptionType>::BuilderArguments& arguments, const std::shared_ptr<TableViewBase>& inOwnerTableView)
		{
			TableRow<OptionType>::init(
				typename TableRow<OptionType>::BuilderArguments()
				.Style(arguments.mStyle)
				.padding(arguments.mpadding)
				.Content
				(
					arguments.mContent->m_childWidget
				),
				inOwnerTableView
			);
		}

		virtual Reply OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override
		{
			//基类的实际类型只有当类的模板实参确定了才能确定
			//必须用 this 或有限定名字访问成员变成待决
			//实例化点才会查找这个名字
			//否则会在定义点查找，而定义点基类看不到
			//定义点是模板定义的位置
			//实例点，编译器会把实例化后的模板特化放在哪里
			//不同的位置会影响名字查找的结果
			std::shared_ptr<ITypedTableView<OptionType>> ownerTable = this->m_ownerTablePtr.lock();

			const OptionType* myItem = ownerTable->privateItemFromWidget(this);
			const bool bIsSelected = ownerTable->privateIsItemSelected(*myItem);

			if (bIsSelected)
			{
				ownerTable->privateSignalSelectionChanged(SelectInfo::Direct);
				return Reply::Handled();
			}

			return TableRow<OptionType>::OnMouseButtonDown(myGeometry, inMouseEvent);
		}
	};

	//a combo box that shows arbitrary content
	template<typename OptionType>
	class ComboBox : public ComboButton
	{
	public:
		using listTypeTraits = ListTypeTraits<OptionType>;//这里以小写的 list 开头
		using NullableOptionType = typename ListTypeTraits<OptionType>::NullableType;

		//list 的类型，被用于展示菜单选项
		using ComboListType = ListView<OptionType>;

		//委托类型，被用于去生成表示选项的控件
		using OnGenerateWidget = typename WidgetDelegates<OptionType>::OnGenerateWidget;
		using OnSelectionChanged = typename WidgetDelegates<NullableOptionType>::OnSelectionChanged;

		struct BuilderArguments : public Arguments<ComboBox>
		{
			BuilderArguments()
				: mcomboBoxStyle(StyleSet::getStyle()->template getStyle<ComboBoxStyle>("comboBox"))
				, mItemStyle(StyleSet::getStyle()->template getStyle<TableRowStyle>("comboBox.Row"))
				, mcontentPadding(mcomboBoxStyle.Get()->m_contentPadding)
				, minitiallySelectedItem(listTypeTraits::makeNullPtr())
				, mmaxListHeight(420.0f)
				, mhasDownArrow(true)
				, menableGamepadNavigationMode(false)
				, misFocusable(true)
			{}

			virtual ~BuilderArguments(){}

			//按钮内容的 slot (这是可选的)
			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)

			ARGUMENT_ATTRIBUTE(std::shared_ptr<ComboBoxStyle>, comboBoxStyle)

			//这个会覆盖掉 combo box style 的风格
			ARGUMENT_ATTRIBUTE(std::shared_ptr<ButtonStyle>, buttonStyle)

			ARGUMENT_ATTRIBUTE(std::shared_ptr<TableRowStyle>, ItemStyle)

			ARGUMENT_ATTRIBUTE(Padding, contentPadding)
			ARGUMENT_ATTRIBUTE(math::float4, foregroundColor)

			ARGUMENT_VALUE(const std::vector<OptionType>*, optionSource)
			UI_EVENT(OnSelectionChanged, onSelectionChanged)
			UI_EVENT(OnGenerateWidget, onGenerateWidget)
			
			UI_EVENT(OnComboBoxOpening, onComboBoxOpening)

			ARGUMENT_VALUE(std::shared_ptr<ScrollBar>, customScrollbar)

			//当 combo box 第一次被创建的时候，应当被选中的 option
			ARGUMENT_VALUE(NullableOptionType, initiallySelectedItem)

			ARGUMENT_VALUE(std::optional<PopupMethod>, method)

			//combo box menu 的最大高度
			ARGUMENT_VALUE(float, maxListHeight)

			ARGUMENT_VALUE(bool, hasDownArrow)

			ARGUMENT_VALUE(bool, enableGamepadNavigationMode)

			ARGUMENT_VALUE(bool, isFocusable)

			//这个 combo 的菜单是否应当被折叠，当我们的父亲接收焦点的时候
			ARGUMENT_VALUE(bool, collapseMenuOnParentFocus)
		};

		void init(const BuilderArguments& arguments)
		{
			m_itemStyle = arguments.mItemStyle.Get();
			m_menuRowPadding = arguments.mcomboBoxStyle.Get()->m_contentPadding;

			std::shared_ptr<ComboButtonStyle> ourComboButtonStyle = arguments.mcomboBoxStyle.Get()->m_comboButtonStyle;
			std::shared_ptr<ButtonStyle> ourButtonStyle = arguments.mbuttonStyle.IsSet() ? arguments.mbuttonStyle.Get() : ourComboButtonStyle->m_buttonStyle;

			this->m_onComboBoxOpening = arguments.monComboBoxOpening;
			this->m_onSelectionChanged = arguments.monSelectionChanged;
			this->m_onGenerateWidget = arguments.monGenerateWidget;
			this->m_bControllerInputCaptured = false;

			m_optionSource = arguments.moptionSource;
			m_customScrollbar = arguments.mcustomScrollbar;

			std::shared_ptr<Widget> comboBoxMenuContent;
			WIDGET_ASSIGN_NEW(BoxWidget, comboBoxMenuContent)
			.MaxDesiredHeight(arguments.mmaxListHeight) //这个来限制高度，当内容大于高度的时候，出现滑条
			.Content
			(
				WIDGET_ASSIGN_NEW(ComboListType, this->m_comboListView)
				.ListItemSource(arguments.moptionSource)
				.onGenerateRow(this, &ComboBox<OptionType>::generateMenuItemRow)
				.onSelectionChanged(this, &ComboBox<OptionType>::onSelectionChangedInternal)
				.selectionMode(SelectionMode::Single)
				.externalScrollbar(arguments.mcustomScrollbar)
			);

			//设置 ComboButton 内容
			std::shared_ptr<Widget> buttonContent = arguments.mContent->m_childWidget;

			if (arguments.mContent->m_childWidget == NullWidget::getNullWidget())
			{
				WIDGET_ASSIGN_NEW(TextBlockWidget, buttonContent)
					.text("no content provided")
					.textColor(math::float4(1.0f, 1.0f, 1.0f, 1.0f));
			}

			ComboButton::init(
				ComboButton::BuilderArguments()
				.comboButtonStyle(ourComboButtonStyle)
				.buttonStyle(ourButtonStyle)
				.method(arguments.mmethod)
				.buttonContent
				(
					buttonContent
				)
				.menuContent
				(
					comboBoxMenuContent
				)
				.hasDownArrow(arguments.mhasDownArrow)
				.contentPadding(arguments.mcontentPadding)
				.isFocusable(arguments.misFocusable)
				//.collapseMenuOnParentFocus(arguments.mcollapseMenuOnParentFocus) //combo button 应该增加一个 on menu open changed
			);

			//setMenuContentWidgetToFocus()

			m_selectedItem = arguments.minitiallySelectedItem;

			if (listTypeTraits::isPtrValid(m_selectedItem))
			{
				OptionType validatedItem = listTypeTraits::nullableItemTypeConverToItemType(m_selectedItem);
				m_comboListView->privateSetItemSelection(validatedItem, true);
				m_comboListView->requestScrollIntoView(validatedItem);
			}
		}

		virtual bool supportsKeyboardFocus() const override
		{
			return m_bIsFocusable;
		}


		NullableOptionType getSelectedItem()
		{
			return m_selectedItem;
		}


		std::shared_ptr<TableRowStyle> m_itemStyle;

		Padding m_menuRowPadding;

	private:
		//被调用，当 list 的选择改变的时候
		//proposed selection 表示建议的选择
		void onSelectionChangedInternal(NullableOptionType proposedSelection, SelectInfo::Type selectInfo)
		{
			if (selectInfo != SelectInfo::OnNavigation)
			{
				if (proposedSelection != m_selectedItem)
				{
					m_selectedItem = proposedSelection;
					if(m_onSelectionChanged)
						m_onSelectionChanged(proposedSelection, selectInfo);
				}

				//关闭 combo ，即使用户重新选择了项目
				this->setIsOpen(false);
			}
		}

		std::shared_ptr<ITableRow> generateMenuItemRow(OptionType inItem, const std::shared_ptr<TableViewBase>& ownerTable)
		{
			if (m_onGenerateWidget)
			{
				return WIDGET_NEW(ComboRow<OptionType>, ownerTable)
					.Style(m_itemStyle)
					.padding(m_menuRowPadding)
					.Content
					(
						m_onGenerateWidget(inItem)
					);
			}
			else
			{
				return WIDGET_NEW(ComboRow<OptionType>, ownerTable)
					.Content
					(
						WIDGET_NEW(TextBlockWidget)
						.text("please provide a on generate widget handler")
					);
			}
		}

	private:
		//当在 combo box 选中的项发生变化的时候，这个委托被调用
		OnSelectionChanged m_onSelectionChanged;
		//现在在 combo box 中被选中的 item
		NullableOptionType m_selectedItem;
		//我们弹出的 list view ，可视化可用的选项
		std::shared_ptr<ComboListType> m_comboListView;
		//使用在 list view 中的 scroll bar
		std::shared_ptr<ScrollBar> m_customScrollbar;
		//在 combo box 被打开的时候，被调用的委托
		OnComboBoxOpening m_onComboBoxOpening;
		
		OnGenerateWidget m_onGenerateWidget;

		//使用激活的按钮，去触发 list view ，当开启的时候
		bool m_enableGamepadNavigationMode;

		bool m_bControllerInputCaptured;

		const std::vector<OptionType>* m_optionSource;
	};
}