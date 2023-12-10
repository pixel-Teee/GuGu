#include <pch.h>

#include "Application.h"

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
        }
    }
    void Application::pumpMessage()
    {
    }
    void Application::setExit(bool value)
    {
        m_alreadyExit = value;
    }
}