#pragma once

#include "Variant.h"

namespace GuGu
{
    namespace meta
    {
        template<typename ClassType>
        void DestructorInvoker<ClassType>::Invoke(const Variant& obj)
        {
            auto &instance = obj.GetValue<ClassType>( );

            instance.~ClassType( );
        }
    }
}
