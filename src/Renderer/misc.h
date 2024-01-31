#pragma once

#include <cstdint>
#include <cassert>

namespace GuGu {
    namespace nvrhi {
        // A type cast that is safer than static_cast in debug builds, and is a simple static_cast in release builds.
        // Used for downcasting various ISomething* pointers to their implementation classes in the backends.
        template<typename T, typename U>
        T checked_cast(U u) {
            static_assert(!std::is_same<T, U>::value, "Redundant checked_cast");
#ifdef _DEBUG
            if (!u) return nullptr;
            T t = dynamic_cast<T>(u);
            if (!t) assert(!"Invalid type cast");  // NOLINT(clang-diagnostic-string-conversion)
            return t;
#else
            return static_cast<T>(u);
#endif
        }
    }
}