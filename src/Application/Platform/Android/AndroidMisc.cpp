#include <pch.h>

#include "AndroidMisc.h"

namespace GuGu{
    bool processAndroidEvents(android_app *app)
    {
        android_poll_source* source;

        int ident;
        int events;

        while((ident = ALooper_pollAll(0, nullptr, &events, (void**)&source)) >= 0)
        {
            if(source)
            {
                source->process(app, source);
            }

            if (app->destroyRequested != 0)
            {
                return false;
            }
        }
        return true;
    }
}