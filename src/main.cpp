#include <Window/Window.h>

#include <Core/GuGuUtf8Str.h>

#ifdef WIN32
#include <Application/Platform/Windows/WindowsApplication.h>
#else 
	#ifdef ANDROID
        #include <Application/Platform/Android/AndroidApplication.h>
        #include <Application/Platform/Android/AndroidMisc.h>
	#endif
#endif

#ifdef WIN32
#include <fstream>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
//int32_t main()
{
	//std::shared_ptr<GuGu::Window> window = GuGu::CreateWindowFactory();
	//
	GuGu::GuGuUtf8Str str(u8"你好呀!我日，想自由");
	////std::cout << str;
	

	if (::GetConsoleWindow() == NULL)
	{
		if (::AllocConsole())
		{
			(void)freopen("CONIN$", "r", stdin);
			(void)freopen("CONOUT$", "w", stdout);
			(void)freopen("CONOUT$", "w", stderr);

			//SetFocus(::GetConsoleWindow());
		}
	}

    system("chcp 65001");
	GuGu_LOGD("%s", str.getStr());
	GuGu_LOGI("%s", str.getStr());
	//std::ofstream f(u8"测试.txt", std::ios_base::out);
	//for (size_t i = 0; i < str.len(); ++i)
	//{
	//	GuGu::GuGuUtf8Str q = str[i];
	//	f << q << u8'\n';
	//}
	//GuGu::GuGuUtf8Str q = str.substr(3, -1);
	//f << q;
	//
	//f.close();

	std::shared_ptr<GuGu::Application> application = GuGu::CreateApplicationFactory();
	std::shared_ptr<GuGu::WindowsApplication> windowsApplication = std::static_pointer_cast<GuGu::WindowsApplication>(application);
	windowsApplication->setNativeApplicationHandleAndCmdShow(hInstance, nCmdShow);

	application->init();
	application->Run();
	return 0;
}
#else
	#ifdef ANDROID
	#include <jni.h>
	#include <game-activity/GameActivity.cpp>
	#include <game-text-input/gametextinput.cpp>
    #include <android/log.h>
    #include <Renderer/Platform/Vulkan/VulkanRenderer.h>
    #include <Core/Platform/Android/AndroidGuGuFile.h>
    //#define LOG_TAG "AndroidLog"
    //#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
extern "C" {
	#include <game-activity/native_app_glue/android_native_app_glue.c>
JNIEXPORT void JNICALL
Java_com_example_gugu_MainActivity_passInternalStorageDataPathAndFilePath(JNIEnv *env, jobject thiz,
                                                                          jstring data_path,
                                                                          jstring file_path) {
    const char* internalStorageDataPath = env->GetStringUTFChars(data_path, NULL);
    const char* internalStorageFilePath = env->GetStringUTFChars(file_path, NULL);
    //ALOGD("internal storage data path : %s", internalStorageDataPath);
    //ALOGD("internal storage file path : %s", internalStorageFilePath);
    GuGu::GuGuUtf8Str dataPath(internalStorageDataPath);
    GuGu::GuGuUtf8Str filePath(internalStorageFilePath);
    GuGu::AndroidGuGuFile::setInternalPath(dataPath, filePath);
    env->ReleaseStringUTFChars(data_path, internalStorageDataPath);
    env->ReleaseStringUTFChars(file_path, internalStorageFilePath);
}
void handle_cmd(android_app *pApp, int32_t cmd) {
    std::shared_ptr<GuGu::AndroidApplication> androidApplication = *reinterpret_cast<std::shared_ptr<GuGu::AndroidApplication>*>(pApp->userData);
    //std::shared_ptr<GuGu::Renderer> renderer = androidApplication->getRenderer();
    //std::shared_ptr<GuGu::VulkanRenderer> vulkanRenderer = std::static_pointer_cast<GuGu::VulkanRenderer>(renderer);
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            // A new window is created, associate a renderer with it. You may replace this with a
            // "game" class if that suits your needs. Remember to change all instances of userData
            // if you change the class here as a reinterpret_cast is dangerous this in the
            // android_main function and the APP_CMD_TERM_WINDOW handler case.
            //pApp->userData = new Renderer(pApp);
            androidApplication->setSurfaceReady(true);
            androidApplication->setAndroidNativeWindow(pApp);

            //androidApplication->setAssetManager(pApp->activity->assetManager);
            GuGu::AndroidGuGuFile::setAssetManager(pApp->activity->assetManager);
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being destroyed. Use this to clean up your userData to avoid leaking
            // resources.
            //
            // We have to check if userData is assigned just in case this comes in really quickly
            //if (pApp->userData) {
            //    //
            //    auto *pRenderer = reinterpret_cast<Renderer *>(pApp->userData);
            //    pApp->userData = nullptr;
            //    delete pRenderer;
            //}
            break;
        default:
            break;
    }
}

//int32_t on_input_event(android_app *pApp, AInputEvent* input_event)
//{
//    std::shared_ptr<GuGu::AndroidApplication> androidApplication = *reinterpret_cast<std::shared_ptr<GuGu::AndroidApplication>*>(pApp->userData);
//
//    std::int32_t event_source = AInputEvent_getSource(input_event);
//
//    return 1;
//}

void android_main(struct android_app *pApp) {
	//ALOGD("Hello, JNI");

    //set native android app
    std::shared_ptr<GuGu::Application> application = GuGu::CreateApplicationFactory();
    std::shared_ptr<GuGu::AndroidApplication> androidApplication = std::static_pointer_cast<GuGu::AndroidApplication>(application);
    androidApplication->setAndroidApp(pApp);

    pApp->onAppCmd = handle_cmd;
    //pApp->onInputEvent = on_input_event;
    pApp->userData = reinterpret_cast<void*>(&androidApplication);

    //------open hole, similar to setNativeApplicationHandleAndCmdShow------
    ALOGD("waiting on window surface to be created ready!")
    do{
        if(!GuGu::processAndroidEvents(pApp))
        {
            ALOGD("android app has been destroyed by the os!");
        }
    }while(!androidApplication->getSurfaceReady());

    androidApplication->init();

    androidApplication->Run();

    // This sets up a typical game/event loop. It will run until the app is destroyed.
    //int events;
    //android_poll_source *pSource;
    //do {
    //    // Process all pending events before running game logic.
    //    if (ALooper_pollAll(0, nullptr, &events, (void **) &pSource) >= 0) {
    //        if (pSource) {
    //            pSource->process(pApp, pSource);
    //        }
    //    }
//
    //    // Check if any user data is associated. This is assigned in handle_cmd
    //    if (pApp->userData) {
//
    //        // We know that our user data is a Renderer, so reinterpret cast it. If you change your
    //        // user data remember to change it here
    //        //auto *pRenderer = reinterpret_cast<Renderer *>(pApp->userData);
//
    //        // Process game input
    //        //pRenderer->handleInput();
//
    //        // Render a frame
    //        //pRenderer->render();
    //    }
    //} while (!pApp->destroyRequested);

}
}
	#endif
#endif
