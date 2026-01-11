#include <Core/Log.h>
#include <Core/GuGuUtf8Str.h>//test utf8 str
#include <Window/Window.h>
#include "GameViewportClient.h"
#include <Core/Reflection/TestReflection.h>
#include <Core/LuaContext/LuaContext.h>
#include <Core/GamePlay/GamePlayerReflectionRegister.h>

#ifdef WIN32
#include <Application/Platform/Windows/WindowsApplication.h>
#else 
#ifdef ANDROID
#include <Application/Platform/Android/AndroidApplication.h>
#include <Application/Platform/Android/AndroidMisc.h>
#endif
#endif

#include <vector>
#include <memory>

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	GuGu::GuGuUtf8Str str(u8"windows platform entry point");

	if (::GetConsoleWindow() == NULL)
	{
		if (::AllocConsole())
		{
			(void)freopen("CONIN$", "r", stdin);
			(void)freopen("CONOUT$", "w", stdout);
			(void)freopen("CONOUT$", "w", stderr);
		}
	}

	system("chcp 65001");//change code page to utf8
	GuGu_LOGD("%s", str.getStr());
	GuGu::testReflection();
	GuGu::gamePlayerReflectionRegister();
	GuGu::LuaContext::getLuaContext()->initialize();
	std::shared_ptr<GuGu::Application> application = GuGu::CreateApplicationFactory();
	std::shared_ptr<GuGu::WindowsApplication> windowsApplication = std::static_pointer_cast<GuGu::WindowsApplication>(application);
	windowsApplication->setNativeApplicationHandleAndCmdShow(hInstance, nCmdShow);

	std::shared_ptr<GuGu::WindowWidget> gameWindowWidget = GuGu::CreateGameMainWindow();
	application->init(gameWindowWidget);
	GuGu::loadLevel();//load default level

	application->Run();
	return 0;
}
#else
#ifdef ANDROID
#include <jni.h>
#include <game-activity/GameActivity.cpp>
#include <game-text-input/gametextinput.cpp>
#include <Core/Platform/Android/AndroidGuGuFile.h>
extern "C" {
#include <game-activity/native_app_glue/android_native_app_glue.h>
	JNIEXPORT void JNICALL
		Java_com_example_gugu_MainActivity_passInternalStorageDataPathAndFilePath(JNIEnv* env, jobject thiz,
			jstring data_path,
			jstring file_path) {
		const char* internalStorageDataPath = env->GetStringUTFChars(data_path, NULL);
		const char* internalStorageFilePath = env->GetStringUTFChars(file_path, NULL);
		GuGu::GuGuUtf8Str dataPath(internalStorageDataPath);
		GuGu::GuGuUtf8Str filePath(internalStorageFilePath);
		GuGu::AndroidGuGuFile::setInternalPath(dataPath, filePath);
		env->ReleaseStringUTFChars(data_path, internalStorageDataPath);
		env->ReleaseStringUTFChars(file_path, internalStorageFilePath);
	}
	void handle_cmd(android_app* pApp, int32_t cmd) {
		std::shared_ptr<GuGu::AndroidApplication> androidApplication = *reinterpret_cast<std::shared_ptr<GuGu::AndroidApplication>*>(pApp->userData);
		switch (cmd) {
		case APP_CMD_INIT_WINDOW:
		{
			androidApplication->setSurfaceReady(true);
			androidApplication->setAndroidNativeWindow(pApp);
			GuGu::AndroidGuGuFile::setAssetManager(pApp->activity->assetManager);
			break;
		}
		case APP_CMD_CONTENT_RECT_CHANGED:
		{
			//get the new size
			auto width = pApp->contentRect.right - pApp->contentRect.left;
			auto height = pApp->contentRect.bottom - pApp->contentRect.top;
			//platform->resize(width, height);
			break;
		}
		case APP_CMD_TERM_WINDOW:
		{
			break;
		}
		case APP_CMD_GAINED_FOCUS:
		{
			GuGu_LOGD("get focus");
			androidApplication->setFocused(true);
			androidApplication->setNeedToRecreateSwapChain(true);
			break;
		}
		case APP_CMD_LOST_FOCUS:
		{
			GuGu_LOGD("lost focus");
			androidApplication->setFocused(false);
			break;
		}
		default:
			break;
		}
	}

	bool key_event_filter(const GameActivityKeyEvent* event)
	{
		if (event->source == AINPUT_SOURCE_KEYBOARD)
		{
			return true;
		}
		return false;
	}

	bool motion_event_filter(const GameActivityMotionEvent* event)
	{
		if ((event->source == AINPUT_SOURCE_MOUSE) ||
			(event->source == AINPUT_SOURCE_TOUCHSCREEN))
		{
			return true;
		}
		return false;
	}

	void on_content_rect_changed(GameActivity* activity, const ARect* rect)
	{
		GuGu_LOGI("ContentRectChanged: {:p}\n", static_cast<void*>(activity));
		struct android_app* app = reinterpret_cast<struct android_app*>(activity->instance);
		auto                cmd = APP_CMD_CONTENT_RECT_CHANGED;

		app->contentRect = *rect;

		if (write(app->msgwrite, &cmd, sizeof(cmd)) != sizeof(cmd))
		{
			GuGu_LOGE("failure writing android_app cmd: {}\n", strerror(errno));
		}
	}

	void android_main(struct android_app* pApp) {

        GuGu::testReflection();
        GuGu::gamePlayerReflectionRegister();
		GuGu::LuaContext::getLuaContext()->initialize();
		std::shared_ptr<GuGu::Application> application = GuGu::CreateApplicationFactory();
		std::shared_ptr<GuGu::AndroidApplication> androidApplication = std::static_pointer_cast<GuGu::AndroidApplication>(application);
		androidApplication->setAndroidApp(pApp);

		android_app_set_key_event_filter(pApp, key_event_filter);
		android_app_set_motion_event_filter(pApp, motion_event_filter);

		pApp->onAppCmd = handle_cmd;
		//pApp->onInputEvent = on_input_event;
		pApp->activity->callbacks->onContentRectChanged = on_content_rect_changed;
		pApp->userData = reinterpret_cast<void*>(&androidApplication);

		//------similar to setNativeApplicationHandleAndCmdShow------
		GuGu_LOGD("waiting on window surface to be created ready!");
		do {
			if (!GuGu::processAndroidEvents(pApp))
			{
				GuGu_LOGD("android app has been destroyed by the os!");
			}
		} while (!androidApplication->getSurfaceReady());
		//------similar to setNativeApplicationHandleAndCmdShow------

		//GuGu::InitArchive();
        std::shared_ptr<GuGu::WindowWidget> gameWindowWidget = GuGu::CreateGameMainWindow();
		androidApplication->init(gameWindowWidget);
		GuGu::loadLevel();//load default level
		androidApplication->Run();

	}
}
#endif
#endif
