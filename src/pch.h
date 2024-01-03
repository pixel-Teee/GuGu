#pragma once

#include <iostream>
#include <cstdio>
#include <cstring>
#include <memory>
#include <cassert>
#include <log.h>
#include <optional>
#include <map>
#include <set>

#ifdef WIN32
//------window platform header file------
#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <Renderer/Platform/D3D12/d3dx12.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#else
    #ifdef ANDROID
        #include <vulkan/vulkan.h>

        #define VK_CHECK(x)                                             \
	    do                                                              \
	    {                                                               \
		    VkResult err = x;                                           \
		    if (err)                                                    \
		    {                                                           \
                GuGu_LOGE("Detected Vulkan error: %d errorCode", err);  \
		    	abort();                                                \
		    }                                                           \
	    } while (0)
    #endif
#endif