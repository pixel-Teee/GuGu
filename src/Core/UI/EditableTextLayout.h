#pragma once

#include "Style.h"
#include "FontCache.h"//TextShapingMethod depends on it
#include "Reply.h"
#include "EditableTextTypes.h"

namespace GuGu {

	class IEditableTextWidget;
	class ITextLayoutMarshaller;
	class GuGuTextLayout;

	struct PaintArgs;
	class ElementList;
	class WidgetGeometry;

	struct KeyEvent;
	struct CharacterEvent;
	struct PointerEvent;

	class MoveCursor;
	//class EditableTextTypes::CursorLineHighlighter;
	class EditableTextLayout
	{
	public:
		EditableTextLayout(IEditableTextWidget& inOwnerWidget, const Attribute<GuGuUtf8Str>& inInitialText, TextBlockStyle inTextStyle, const std::optional<TextShapingMethod> inTextShapingMethod,
			std::shared_ptr<ITextLayoutMarshaller> inMarshaller);

		~EditableTextLayout();

		const TextBlockStyle& getTextStyle() const;

		void cacheDesiredSize(float layoutScaleMultiplier);

		math::float2 ComputeFixedSize(float layoutScaleMultiplier) const;

		int32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer);

		void Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime);

		bool refresh();

		bool refreshImpl(const GuGuUtf8Str& inTextToSet);

		Reply handleKeyChar(const CharacterEvent& inCharacterEvent);

		Reply handleKeyDown(const KeyEvent& inKeyEvent);

		Reply handleMouseButtonDown(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent);

		Reply handleMouseButtonUp(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent);

		Reply handleMouseMove(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent);

		void handleFocusLost();

		bool handleFocusReceived();

		bool handleTypeChar(const GuGuUtf8Str& inChar);	

		bool handleCarriageReturn();

		GuGuUtf8Str getEditableText() const;

		bool setEditableText(const GuGuUtf8Str& textToSet);

		bool moveCursor(const MoveCursor& inArgs);

		bool handleBackspace();

	private:
		/*插入给定的文本在现在的光标位置*/
		void insertTextAtCursorImpl(const GuGuUtf8Str& inString);
		
		/*更新激活的光标高亮，基于文本布局的状态*/
		void updateCursorHighlight();

		/*拷贝选择到的文本到剪切板*/
		void copySelectedTextToClipboard();

		/*从剪切板粘贴文本*/
		void pasteTextFromClipboard();

		bool anyTextSelected() const;

		/*选中所有的文本*/
		void selectAllText();

		//删除任何现在选择的文本
		void deleteSelectedText();

		//删除激活的文本选择
		void clearSelection();

		//给定一个位置还有一个方向去偏移，返回一个新位置
		TextLocation translatedLocation(const TextLocation& currentLocation, int8_t direction) const;

		IEditableTextWidget* m_ownerWidget;

		std::shared_ptr<ITextLayoutMarshaller> m_marshaller;

		std::shared_ptr<GuGuTextLayout> m_textLayout;

		Attribute<GuGuUtf8Str> m_boundText;

		TextBlockStyle m_textStyle;

		math::float2 m_cachedSize;

		EditableTextTypes::CursorInfo m_cursorInfo;

		std::shared_ptr<EditableTextTypes::CursorLineHighlighter> m_cursorLineHighlighter;

		std::shared_ptr<EditableTextTypes::TextSelectionHighlighter> m_textSelectionHighlighter;

		std::vector<TextLineHighlight> m_activeLineHighlights;

		//如果选择了文本通过使用左键拖动鼠标光标，那这个值为真
		bool m_bIsDragSelecting;

		//选择的开始的地方，结束的地方是光标所在的位置
		std::optional<TextLocation> m_selectionStart;

		//上一次tick的bound text
		GuGuUtf8Str m_boundTextLastTick;

		//scroll offset(在未缩放的单元)对于这个文本
		math::float2 m_scrollOffset;

		//如果被设置了，包含一个位置的待处理数据应当被滑进view里面
		std::optional<EditableTextTypes::ScrollInfo> m_positionToScrollIntoView;
	};
}