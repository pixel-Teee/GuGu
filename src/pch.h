#pragma once

#include <iostream>
#include <cstdio>
#include <cstring>
#include <memory>

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
#endif