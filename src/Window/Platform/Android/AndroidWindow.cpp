#include <pch.h>
#include "AndroidWindow.h"

#include <jni.h>
#include <game-activity/GameActivity.h>

float dpiFactor = 1.0f;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_gugu_MainActivity_setDpiScale(JNIEnv *env, jobject thiz, jfloat dpi_scale) {
    dpiFactor = dpi_scale;
}

namespace GuGu{
    std::shared_ptr<Window> CreateWindowFactory()
    {
        return std::make_shared<AndroidWindow>();
    }

    void AndroidWindow::ToGeneratePlatformWindow() {
        //android nothing to do
    }

    float AndroidWindow::getDpiFactor() {
        return dpiFactor;
    }

    math::float2 AndroidWindow::getWindowScreenSpacePosition() {
        return math::float2(0.0f, 0.0f);
    }

    void AndroidWindow::setNativeWindow(ANativeWindow *nativeWindow) {
        m_nativeWindow = nativeWindow;
    }

    ANativeWindow *AndroidWindow::getNativeHandle() const {
        return m_nativeWindow;
    }
}