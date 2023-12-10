#include <pch.h>
#include "AndroidWindow.h"

namespace GuGu{
    std::shared_ptr<Window> CreateWindowFactory()
    {
        return std::make_shared<AndroidWindow>();
    }

    void AndroidWindow::ToGeneratePlatformWindow() {
        //android nothing to do
    }

    void AndroidWindow::setNativeWindow(ANativeWindow *nativeWindow) {
        m_nativeWindow = nativeWindow;
    }
}