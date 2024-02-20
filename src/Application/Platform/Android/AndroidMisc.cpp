#include <pch.h>

#include "AndroidMisc.h"
#include "AndroidApplication.h"

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

            //auto input_buf = android_app_swap_input_buffers(app);
            //if (!input_buf)
            //{
            //    //return;
            //}
            //if (input_buf->motionEventsCount)
            //{
            //    for (int idx = 0; idx < input_buf->motionEventsCount; idx++)
            //    {
            //        auto event = &input_buf->motionEvents[idx];
            //        assert((event->source == AINPUT_SOURCE_MOUSE ||
            //                event->source == AINPUT_SOURCE_TOUCHSCREEN) &&
            //               "Invalid motion event source");
//
            //        std::int32_t action = event->action;
//
            //        float x = GameActivityPointerAxes_getX(&event->pointers[0]);
            //        float y = GameActivityPointerAxes_getY(&event->pointers[0]);
//
            //        if (event->source == AINPUT_SOURCE_MOUSE)
            //        {
            //            input_event(MouseButtonInputEvent{
            //                    translate_mouse_button(0),
            //                    translate_mouse_action(action),
            //                    x, y});
            //        }
            //        else if (event->source == AINPUT_SOURCE_TOUCHSCREEN)
            //        {
            //            // Multiple pointers are not supported.
            //            size_t       pointer_count = event->pointerCount;
            //            std::int32_t pointer_id    = event->pointers[0].id;
//
            //            input_event(TouchInputEvent{
            //                    pointer_id,
            //                    pointer_count,
            //                    translate_touch_action(action),
            //                    x, y});
            //        }
            //    }
            //    android_app_clear_motion_events(input_buf);
            //}
//
            //if (input_buf->keyEventsCount)
            //{
            //    for (int idx = 0; idx < input_buf->keyEventsCount; idx++)
            //    {
            //        auto event = &input_buf->keyEvents[idx];
            //        assert((event->source == AINPUT_SOURCE_KEYBOARD) &&
            //               "Invalid key event source");
            //        input_event(KeyInputEvent{
            //                translate_key_code(event->keyCode),
            //                translate_key_action(event->action)});
            //    }
            //    android_app_clear_key_events(input_buf);
            //}
        }
        return true;
    }
}