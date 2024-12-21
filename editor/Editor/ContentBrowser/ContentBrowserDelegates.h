#pragma once

#include <functional>
#include <Core/UI/BasicElement.h>
#include "ContentBrowserItem.h"

//当项被选中的时候，委托被调用
using OnContentBrowserItemSelectionChanged = std::function<void(const GuGu::ContentBrowserItem&, GuGu::SelectInfo::Type)>;