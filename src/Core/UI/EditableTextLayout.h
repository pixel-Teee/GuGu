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

		bool handleTypeChar(const GuGuUtf8Str& inChar);

		GuGuUtf8Str getEditableText() const;

		bool moveCursor(const MoveCursor& inArgs);

		bool handleBackspace();
	private:
		/*更新激活的光标高亮，基于文本布局的状态*/
		void updateCursorHighlight();

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

		std::vector<TextLineHighlight> m_activeLineHighlights;
	};
}