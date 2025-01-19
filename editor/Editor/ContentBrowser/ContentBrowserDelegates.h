#pragma once

#include <functional>
#include <Core/GuGuUtf8Str.h>
#include <Core/UI/Widget.h>
#include <Core/AssetManager/AssetData.h>
#include <Core/AssetManager/AssetManager.h>

//当一个路径选择器里面的路径选择的时候，调用
using OnPathSelected = std::function<void(const GuGu::GuGuUtf8Str&)>;

using OnGetFolderContextMenu = std::function<std::shared_ptr<GuGu::Widget>(const std::vector<GuGu::GuGuUtf8Str>&)>;
using OnGetAssetContextMenu = std::function<std::shared_ptr<GuGu::Widget>(const std::vector<GuGu::AssetData>&)>;