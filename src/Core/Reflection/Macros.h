#pragma once

#if defined(__REFLECTION_PARSER__)

    #define Meta(...) __attribute__((annotate(#__VA_ARGS__)))

    #define __META_EXTERNAL(type, guid)       \
        typedef type __META_EXTERNAL__##guid; \

    #define _META_EXTERNAL(type, guid) __META_EXTERNAL(type, guid)

    #define MetaExternal(type) _META_EXTERNAL(type, __COUNTER__)

    #define META_OBJECT

#else 

    #define Meta(...) 

    #define MetaExternal(type)

    #define MetaInitialize(initializer)                               \
        {                                                             \
            auto &db = GuGu::meta::ReflectionDatabase::Instance( ); \
            initializer;                                              \
        }                                                             \

    //在对象中用于保留虚继承功能
    #define META_OBJECT                                  \
        GuGu::meta::Type GetType(void) const override    \
        {                                                \
            return typeof( decltype( *this ) );          \
        }                                                \
        GuGu::meta::Object* Clone(void) const override   \
        {                                                \
            typedef                                      \
            std::remove_const<                           \
                std::remove_reference<                   \
                    decltype( *this )                    \
                >::type                                  \
            >::type ClassType;                           \
            return new ClassType( *this );               \
        }                                                \

#endif