#include <pch.h>
#include "DragDropOperation.h"

#include "Widget.h"
#include "WindowWidget.h"
#include <Application/Application.h>

namespace GuGu {
    std::shared_ptr<Widget> DragDropOperation::getDefaultDecorator() const
    {
        return std::shared_ptr<Widget>();
    }
    void DragDropOperation::onDragged(const DragDropEvent& dragDropEvent)
    {
        if (m_cursorDecoratorWindow)
        {
            std::shared_ptr<Application> application = Application::getApplication();

            //+ cursor size
            //todo:后续修复改成增加光标大小
            math::float2 position = dragDropEvent.m_screenSpacePosition + math::float2(1.0f, 1.0f);

            if (position.x != m_cursorDecoratorWindow->getPositionInScreen().x ||
                position.y != m_cursorDecoratorWindow->getPositionInScreen().y)
            {
                math::box2 anchor;
                anchor.m_mins = math::float2(position.x, position.y);
                anchor.m_maxs = math::float2(position.x, position.y);
                application->calculatePopupWindowPosition(anchor, m_cursorDecoratorWindow->getFixedSize(), false);
            }

            m_cursorDecoratorWindow->moveWindowTo(position);
        }
    }
    void DragDropOperation::onDrop(bool bDropWasHandled, const PointerEvent& mouseEvent)
    {
        destroyCursorDecoratorWindow();
    }
    void DragDropOperation::destroyCursorDecoratorWindow()
    {
        if (m_cursorDecoratorWindow)
        {
            m_cursorDecoratorWindow->requestDestroyWindow();
            m_cursorDecoratorWindow.reset();
        }
    }
    void DragDropOperation::construct()
    {
        if (m_bCreateNewWindow)
        {
            createCursorDecoratorWindow();
        }
    }
    void DragDropOperation::createCursorDecoratorWindow()
    {
        std::shared_ptr<Widget> decoratorToUse = getDefaultDecorator();
        if (decoratorToUse)
        {
            m_cursorDecoratorWindow = WindowWidget::makeCursorDecorator();
            m_cursorDecoratorWindow->setContent(decoratorToUse);

			Application::getApplication()->makeWindow(m_cursorDecoratorWindow);
			Application::getApplication()->showWindow(m_cursorDecoratorWindow);
        }
    }
}