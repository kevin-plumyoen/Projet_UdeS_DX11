#ifndef DEF_WIN32_BACKEND_TRAITS
#define DEF_WIN32_BACKEND_TRAITS

#include <type_traits>

namespace PM3D {

template<typename T>
struct is_backend {
    static constexpr const bool value = false;
};

template<typename T>
constexpr bool is_backend_v = is_backend<T>::value;

template<typename T>
struct backend_traits;

}

#endif