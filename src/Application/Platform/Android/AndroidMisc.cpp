#include <pch.h>

#include "AndroidMisc.h"
#include "AndroidApplication.h"
#include <Window/Platform/Android/AndroidWindow.h>

namespace GuGu{
    bool processAndroidEvents(android_app *app) //return value is exit or already get event
    {
        android_poll_source* source;

        int ident;
        int events;

        while((ident = ALooper_pollAll(0, nullptr, &events, (void**)&source)) >= 0)//peek message
        {
            if(source)
            {
                source->process(app, source);//dispatch message
            }

            if (app->destroyRequested != 0)
            {
                return false;
            }
        }
        if(app->userData)
        {
            std::shared_ptr<GuGu::AndroidApplication> androidApplication = *reinterpret_cast<std::shared_ptr<GuGu::AndroidApplication>*>(app->userData);
            //handle event

            auto input_buf = android_app_swap_input_buffers(app);
            if (input_buf)
            {
                for (int idx = 0; idx < input_buf->motionEventsCount; idx++)
                {
                    auto event = &input_buf->motionEvents[idx];
                    assert((event->source == AINPUT_SOURCE_MOUSE ||
                            event->source == AINPUT_SOURCE_TOUCHSCREEN) &&
                           "Invalid motion event source");

                    std::int32_t action = event->action;

                    float x = GameActivityPointerAxes_getX(&event->pointers[0]);
                    float y = GameActivityPointerAxes_getY(&event->pointers[0]);

                    if (event->source == AINPUT_SOURCE_TOUCHSCREEN)
                    {
                        if(action == AMOTION_EVENT_ACTION_DOWN)
                        {
                            //find native window
                            std::shared_ptr<AndroidWindow> window = androidApplication->getPlatformWindow();

                            androidApplication->onMouseDown(window, MouseButtons::Left, math::float2(x, y));

                            //GuGu_LOGD("(%f, %f)", x, y);
                        }
                        else if(action == AMOTION_EVENT_ACTION_UP)
                        {
                            std::shared_ptr<AndroidWindow> window = androidApplication->getPlatformWindow();

                            androidApplication->onMouseUp(window, MouseButtons::Left, math::float2(x, y));

                            //GuGu_LOGD("(%f, %f)", x, y);
                        }
                        else if(action == AMOTION_EVENT_ACTION_MOVE)
                        {
                            std::shared_ptr<AndroidWindow> window = androidApplication->getPlatformWindow();

                            androidApplication->onMouseMove(window, math::float2(x, y));
                        }
                    }
                }
                android_app_clear_motion_events(input_buf);
            }
        }

        if(app->textInputState)
        {
            app->textInputState = 0;
        }
        return true;
    }

    void AndroidMisc::ClipboardCopy(const GuGuUtf8Str &str) {

    }

    void AndroidMisc::ClipboardPaste(GuGuUtf8Str &str) {

    }

    void AndroidMisc::getSaveOrOpenFilePathAndFileName(std::shared_ptr<WindowWidget> ownerWindow, const GuGuUtf8Str& initDir, GuGuUtf8Str& filePath, GuGuUtf8Str& fileName, const std::vector<GuGuUtf8Str>& filterArray)
    {

    }
}