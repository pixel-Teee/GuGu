#pragma once

#include "Widget.h"

namespace GuGu {
	class Slot;
	class Window;
	class ElementList;
	class WidgetGeometry;
	class Brush;
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

		void assocateWithNativeWindow(std::shared_ptr<Window> nativeWindow);

		virtual void GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::double2 ComputeFixedSize();

		virtual void AllocationChildActualSpace();

		virtual std::shared_ptr<Slot> getSlot(uint32_t index) override;

		virtual uint32_t getSlotsNumber() override;

		math::int2 getViewportSize();
	public:
		std::weak_ptr<Window> m_nativeWindow;
		std::shared_ptr<Slot> m_childWidget;
		WindowType m_windowType;
		std::shared_ptr<Brush> m_defaultBrush;
	};
}