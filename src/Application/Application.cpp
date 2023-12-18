#include <pch.h>

#include "Application.h"

#include "Renderer/Renderer.h"

namespace GuGu{
    Application::Application()
    {
        m_alreadyExit = false;
    }
    void Application::Run()
    {
        //todo:add exit check
        while (!m_alreadyExit)
        {
            //todo:add update
            bool handlingMessage = pumpMessage();

            //m_renderer->onRender();

            if (!handlingMessage)
                m_renderer->onRender();
        }

        m_renderer->onDestroy();
    }
    bool Application::pumpMessage()
    {
        return false;
    }
    void Application::setExit(bool value)
    {
        m_alreadyExit = value;
    }
    void Application::init()
    {
    }
}