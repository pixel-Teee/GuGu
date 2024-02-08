#include <pch.h>

#include "Application.h"

#include "Renderer/Renderer.h"

#include <Core/Timer.h>

namespace GuGu{
    Application::Application()
    {
        m_alreadyExit = false;
        m_timer = CreateTimerFactory();
    }
    void Application::Run()
    {
        //todo:add exit check
        while (!m_alreadyExit)
        {
            //todo:add update
            pumpMessage();

            m_timer->Tick();
            calculateFrameStats();

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

    std::shared_ptr<Timer> Application::getTimer() {
        return m_timer;
    }

    void Application::calculateFrameStats()
    {
        static int32_t frameCnt = 0;
        static float timeElapsed = 0.0f;

        ++frameCnt;

        if (m_timer->GetTotalTime() - timeElapsed >= 1.0f)
        {
            float fps = (float)frameCnt;
            float mfps = 1000.0f / fps;

            frameCnt = 0;
            timeElapsed += 1.0f;
        }
    }

    //void Application::resize(int32_t width, int32_t height) {
//
    //}
}