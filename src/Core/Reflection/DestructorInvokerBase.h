#pragma once

#include "InvokableConfig.h"

namespace GuGu
{
    namespace meta
    {
        class Variant;

        class DestructorInvokerBase
        {
        public:
            virtual ~DestructorInvokerBase(void) { }

            virtual void Invoke(const Variant &obj) = 0;
        };
    }
}