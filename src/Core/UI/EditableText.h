#pragma once

#include "Widget.h"
#include "UIMacros.h"
#include "IEditableTextWidget.h"
#include "EditableTextLayout.h"
#include "WidgetDelegates.h"

namespace GuGu {
	class PlainTextLayoutMarshaller;
	class EditableText : public Widget, public IEditableTextWidget
	{
	public:
		struct BuilderArguments : public Arguments<EditableText>
		{
			BuilderArguments()
				: mSelecAllTextWhenFocus(false)
				, mSelectAllTextOnCommit(false)
			{
				mClip = WidgetClipping::ClipToBounds;
			}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(GuGuUtf8Str, text)

			ARGUMENT_ATTRIBUTE(bool, SelecAllTextWhenFocus)

			ARGUMENT_ATTRIBUTE(bool, SelectAllTextOnCommit)

			UI_EVENT(OnIsTypedCharValid, onIsTypedCharValid)//会在输入之前，调用这个回调，去过滤不符合的字符

			UI_EVENT(OnTextCommitted, onTextCommitted)//文本提交的回调
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual void cacheDesiredSize(float inLayoutScaleMultiplier) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime) override;

		virtual Reply OnMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;

		virtual Reply OnMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;

		virtual Reply OnMouseMove(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent) override;

		virtual Reply OnKeyChar(const WidgetGeometry& myGeometry, const CharacterEvent& inCharacterEvent) override;

		virtual Reply OnKeyDown(const WidgetGeometry& myGeometry, const KeyEvent& inKeyEvent) override;

		virtual bool supportsKeyboardFocus() const override;

		virtual Reply OnFocusReceived(const WidgetGeometry& myGeometry) override;

		virtual void OnFocusLost() override;

		virtual std::shared_ptr<Widget> getWidget() override;

		virtual bool canTypeCharacter(const GuGuUtf8Str inChar) const override;

		virtual void onTextCommitted(const GuGuUtf8Str& inText, const TextCommit::Type inTextAction) override;

		virtual bool shouldSelectAllTextWhenFocused() const override;
		
		virtual bool shouldSelectAllTextOnCommit() const override;
	protected:
		//填充TextLayout的Marshaller(装配器)，TextLayout是根据一串字符串处理成渲染可用的信息的集合体
		std::shared_ptr<PlainTextLayoutMarshaller> m_plainTextMarshaller;

		//editable text layout间接地处理TextLayout
		std::unique_ptr<EditableTextLayout> m_editableTextLayout;

		//当获取焦点的时候，是否选中所有文本，适用于spin box
		Attribute<bool> m_bSelectAllTextWhenFocus;

		//是否去选择所有的文本，当按下 enter 去提交修改
		Attribute<bool> m_bSelectAllTextOnCommit;

		//当一个字母被输入，并且我们想知道是否文本输入框支持这个字母的时候被调用
		OnIsTypedCharValid m_OnIsTypedCharValid;

		//当文本被提交的时候，发生在用户按下enter或者文本失去焦点的时候被调用
		OnTextCommitted m_OnTextCommittedCallback;
	};
}