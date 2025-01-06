#pragma once

#include <game-activity/native_app_glue/android_native_app_glue.h>

#include <Application/ApplicationMisc.h>

namespace GuGu{
    bool processAndroidEvents(android_app *app);

    struct AndroidMisc : public ApplicationMisc
    {
        static void ClipboardCopy(const GuGuUtf8Str& str);
        static void ClipboardPaste(GuGuUtf8Str& str);

        static void getOpenFilePathAndFileName(std::shared_ptr<WindowWidget> ownerWindow, const GuGuUtf8Str& initDir, GuGuUtf8Str& filePath, GuGuUtf8Str& fileName, const std::vector<GuGuUtf8Str>& filterArray);
    };

    typedef AndroidMisc PlatformMisc;
}