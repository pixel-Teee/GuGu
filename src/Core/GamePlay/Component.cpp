#include <pch.h>

#include "Component.h"

namespace GuGu {
    meta::Type Component::GetType() const
    {
        return typeof(Component);
    }
}