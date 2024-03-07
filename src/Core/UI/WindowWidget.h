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

		struct BuilderArguments
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			ARGUMENT_VALUE(WindowType, Type)

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::double2 ComputeFixedSize(float inLayoutScaleMultiplier) override;

		virtual void AllocationChildActualSpace(WidgetGeometry& allocatedGeometry, ArrangedWidgetArray& arrangedWidgetArray) override;

		virtual SlotBase* getSlot(uint32_t index) override;

		virtual uint32_t getSlotsNumber() override;

		math::int2 getViewportSize();

		void setChildWidget(std::shared_ptr<Widget> widget);

		void assocateWithNativeWindow(std::shared_ptr<Window> nativeWindow);

		std::shared_ptr<Window> getNativeWindow();

		//static Slot::SlotBuilderArguments SingleSlot() { return Slot::SlotBuilderArguments(std::make_shared<Slot>()); };
	private:
		std::weak_ptr<Window> m_nativeWindow;
		std::shared_ptr<SingleChildSlot> m_childWidget;
		WindowType m_windowType;
		std::shared_ptr<Brush> m_defaultBrush;//for window background
	};
}