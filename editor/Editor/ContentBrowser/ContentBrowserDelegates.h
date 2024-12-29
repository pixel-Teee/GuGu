#pragma once

#include <functional>
#include <Core/GuGuUtf8Str.h>

//当一个路径选择器里面的路径选择的时候，调用
using OnPathSelected = std::function<void(const GuGu::GuGuUtf8Str&)>;