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
            pumpMessage();


            if(m_focused)
            {
                m_renderer->onRender();
            }
            //m_renderer->onRender();
        }

        //m_renderer->onDestroy();
    }
    void Application::pumpMessage()
    {

    }
    void Application::setExit(bool value)
    {
        m_alreadyExit = value;
    }
    void Application:: init()
    {
    }

    std::shared_ptr<Renderer> Application::getRenderer() {
        return m_renderer;
    }

    void Application::setFocused(bool value) {
        m_focused = value;
    }

    bool Application::getFocused() {
        return m_focused;
    }

    //void Application::resize(int32_t width, int32_t height) {
//
    //}
}