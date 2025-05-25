#pragma once

#include <iostream>
#include <cstdio>
#include <cstring>
#include <memory>
#include <cassert>
#include <optional>
#include <map>
#include <set>
#include <Core/Log.h>

#include <Core/Reflection/Priority.h>
#include <Core/Reflection/ReflectionMain.h>
#include <Core/Reflection/ReflectionMacro.h>
#include <Core/Reflection/ReflectionDatabase.h>

#ifdef WIN32
//------window platform header file------
#define NOMINMAX
#if USE_DX12
	#include <d3d12.h>
	#include <d3dcompiler.h>
	#include <dxgi1_6.h>
	#include <wrl.h>
	#include <Renderer/Platform/D3D12/d3dx12.h>
	#pragma comment(lib, "d3d12.lib")
	#pragma comment(lib, "dxgi.lib")
	#pragma comment(lib, "d3dcompiler.lib")
#else USE_VK
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
		//=== VK_EXT_debug_marker ===
		//PFN_vkDebugMarkerSetObjectTagEXT vkDebugMarkerSetObjectTagEXT = 0;
		//PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectNameEXT = 0;
		//PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBeginEXT = 0;
		//PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEndEXT = 0;
		//PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsertEXT = 0;
		//vkDebugMarkerSetObjectTagEXT = PFN_vkDebugMarkerSetObjectTagEXT( vkGetDeviceProcAddr( device, "vkDebugMarkerSetObjectTagEXT" ) );
		//vkDebugMarkerSetObjectNameEXT = PFN_vkDebugMarkerSetObjectNameEXT( vkGetDeviceProcAddr( device, "vkDebugMarkerSetObjectNameEXT" ) );
		//vkCmdDebugMarkerBeginEXT = PFN_vkCmdDebugMarkerBeginEXT( vkGetDeviceProcAddr( device, "vkCmdDebugMarkerBeginEXT" ) );
		//vkCmdDebugMarkerEndEXT = PFN_vkCmdDebugMarkerEndEXT( vkGetDeviceProcAddr( device, "vkCmdDebugMarkerEndEXT" ) );
		//vkCmdDebugMarkerInsertEXT = PFN_vkCmdDebugMarkerInsertEXT( vkGetDeviceProcAddr( device, "vkCmdDebugMarkerInsertEXT" ) );
    #endif
#endif