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
        }
        return true;
    }
}