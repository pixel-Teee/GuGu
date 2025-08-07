#pragma once

#include "DestructorInvokerBase.h"

namespace GuGu
{
    namespace meta
    {
        template<typename ClassType>
        class DestructorInvoker : public DestructorInvokerBase
        {
        public:
            void Invoke(const Variant &obj) override;
        };
    }
}

#include "DestructorInvoker.hpp"