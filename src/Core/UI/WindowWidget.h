#pragma once

#include "Slot.h"
#include "Widget.h"
#include "UIMacros.h"

namespace GuGu {
	//class Slot;
	class Brush;
	class ElementList;
	class WidgetGeometry;
	class ArrangedWidgetArray;

	class Window;

	class WindowWidget;

	using OnWindowClosed = std::function<void(const std::shared_ptr<WindowWidget>&)>;
	
	class WindowWidget : public Widget
	{
	public:
		enum WindowType
		{
			VirtualWindow,
			NativeWindow
		};
		WindowWidget();

		virtual ~WindowWidget();

		struct BuilderArguments : public Arguments<WindowWidget>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			ARGUMENT_VALUE(WindowType, Type)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(const WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) const override;

		virtual SlotBase* getSlot(uint32_t index) const override;

		virtual uint32_t getSlotsNumber() const override;

		math::int2 getViewportSize();

		void setChildWidget(std::shared_ptr<Widget> widget);

		void assocateWithNativeWindow(std::shared_ptr<Window> nativeWindow);

		std::shared_ptr<Window> getNativeWindow();

		void setCachedScreenPosition(math::float2 newPosition);

		void setCachedSize(math::float2 newSize);

		math::float2 getPositionInScreen() const;

		WidgetGeometry getWindowGeometryInScreen() const;

		//static Slot::SlotBuilderArguments SingleSlot() { return Slot::SlotBuilderArguments(std::make_shared<Slot>()); };

		math::box2 getClientRectInScreen() const;

		void moveWindowTo(math::float2 newPosition);
	private:
		std::shared_ptr<Window> m_nativeWindow;
		std::shared_ptr<SingleChildSlot> m_childWidget;
		WindowType m_windowType;
		std::shared_ptr<Brush> m_defaultBrush;//for window background

		math::float2 m_screenPosition;//窗口的屏幕坐标

		math::float2 m_size;//窗口在屏幕空间的内容区域的大小
	};
}