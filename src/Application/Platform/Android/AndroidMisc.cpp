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

                    if (event->source == AINPUT_SOURCE_TOUCHSCREEN)
                    {
                        // Extract the action and the pointer index encoded in the action field
                        int32_t actionMasked = action & AMOTION_EVENT_ACTION_MASK;
                        int32_t pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                                               >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

                        if (actionMasked == AMOTION_EVENT_ACTION_DOWN ||
                            actionMasked == AMOTION_EVENT_ACTION_POINTER_DOWN)
                        {
                            float px = GameActivityPointerAxes_getX(&event->pointers[pointerIndex]);
                            float py = GameActivityPointerAxes_getY(&event->pointers[pointerIndex]);
                            std::shared_ptr<AndroidWindow> window = androidApplication->getPlatformWindow();
                            androidApplication->onTouchDown(window, math::float2(px, py), pointerIndex);
                        }
                        else if (actionMasked == AMOTION_EVENT_ACTION_UP ||
                                 actionMasked == AMOTION_EVENT_ACTION_POINTER_UP)
                        {
                            float px = GameActivityPointerAxes_getX(&event->pointers[pointerIndex]);
                            float py = GameActivityPointerAxes_getY(&event->pointers[pointerIndex]);
                            std::shared_ptr<AndroidWindow> window = androidApplication->getPlatformWindow();
                            androidApplication->onTouchUp(window, math::float2(px, py), pointerIndex);
                        }
                        else if (actionMasked == AMOTION_EVENT_ACTION_MOVE)
                        {
                            // MOVE events carry updated positions for ALL active pointers
                            std::shared_ptr<AndroidWindow> window = androidApplication->getPlatformWindow();
                            for (int32_t pi = 0; pi < event->pointerCount; ++pi)
                            {
                                float px = GameActivityPointerAxes_getX(&event->pointers[pi]);
                                float py = GameActivityPointerAxes_getY(&event->pointers[pi]);
                                androidApplication->onTouchMoved(window, math::float2(px, py), pi);
                            }
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

    void AndroidMisc::getOpenFilePathAndFileName(std::shared_ptr<WindowWidget> ownerWindow, const GuGuUtf8Str& initDir, GuGuUtf8Str& filePath, GuGuUtf8Str& fileName, const std::vector<GuGuUtf8Str>& filterArray)
    {

    }
}