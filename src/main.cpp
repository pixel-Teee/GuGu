#include <Window/Window.h>

#ifdef WIN32
#include <Application/Platform/Windows/WindowsApplication.h>
#else 
	#ifdef ANDROID
	
	#endif
#endif

#ifdef WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	//std::shared_ptr<GuGu::Window> window = GuGu::CreateWindowFactory();

	std::shared_ptr<GuGu::Application> application = GuGu::CreateApplicationFactory();
	std::shared_ptr<GuGu::WindowsApplication> windowsApplication = std::static_pointer_cast<GuGu::WindowsApplication>(application);
	windowsApplication->setNativeApplicationHandleAndCmdShow(hInstance, nCmdShow);

	application->Run();
	return 0;
}
#else
	#ifdef ANDROID
int main()
{
	//std::cout << "Hello World" << std::endl;




	return 0;
}
	#endif
#endif
