#pragma once

#include <game-activity/native_app_glue/android_native_app_glue.h>

#include <Application/ApplicationMisc.h>

namespace GuGu{
    bool processAndroidEvents(android_app *app);

    struct AndroidMisc : public ApplicationMisc
    {
        static void ClipboardCopy(const GuGuUtf8Str& str);
        static void ClipboardPaste(GuGuUtf8Str& str);

        static void getSaveFilePathAndFileName(std::shared_ptr<WindowWidget> ownerWindow, GuGuUtf8Str& filePath, GuGuUtf8Str& fileName);
    };

    typedef AndroidMisc PlatformMisc;
}