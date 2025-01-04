#pragma once

#include <functional>
#include <Core/GuGuUtf8Str.h>
#include <Core/UI/Widget.h>

//当一个路径选择器里面的路径选择的时候，调用
using OnPathSelected = std::function<void(const GuGu::GuGuUtf8Str&)>;

using OnGetAssetContextMenu = std::function<std::shared_ptr<GuGu::Widget>()>;