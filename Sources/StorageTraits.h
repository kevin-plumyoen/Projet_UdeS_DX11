#ifndef DEF_ECS_STORAGE_TRAITS_H
#define DEF_ECS_STORAGE_TRAITS_H

template<typename T>
struct is_storage {
    static constexpr const bool value = false;
};

template<typename T>
constexpr bool is_storage_v = is_storage<T>::value;

#endif