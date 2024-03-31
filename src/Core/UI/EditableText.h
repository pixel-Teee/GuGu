#pragma once

#include "Widget.h"
#include "UIMacros.h"
#include "IEditableTextWidget.h"
#include "EditableTextLayout.h"

namespace GuGu {
	class PlainTextLayoutMarshaller;
	class EditableText : public Widget, public IEditableTextWidget
	{
	public:
		struct BuilderArguments : public Arguments<EditableText>
		{
			BuilderArguments()
			{
				mClip = WidgetClipping::Inherit;
			}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(GuGuUtf8Str, text);
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

		virtual void OnFocusLost() override;

		virtual std::shared_ptr<Widget> getWidget() override;
	protected:
		std::shared_ptr<PlainTextLayoutMarshaller> m_plainTextMarshaller;

		std::unique_ptr<EditableTextLayout> m_editableTextLayout;
	};
}