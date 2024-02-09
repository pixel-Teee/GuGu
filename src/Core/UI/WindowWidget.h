#pragma once

#include "Widget.h"

namespace GuGu {
	class Slot;
	class Window;
	class ElementList;
	class WidgetGeometry;
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

		virtual void GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry);

		virtual math::double2 ComputeFixedSize();

		virtual void AllocationChildActualSpace();

	public:
		std::weak_ptr<Window> m_nativeWindow;
		std::shared_ptr<Slot> m_childWidget;
		WindowType m_windowType;
	};
}