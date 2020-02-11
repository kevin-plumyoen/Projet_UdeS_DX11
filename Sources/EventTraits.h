#ifndef DEF_EVENT_TRAITS_H
#define DEF_EVENT_TRAITS_H

#include <type_traits>

template<typename T>
struct is_event : std::false_type {};

template<typename T>
inline constexpr bool is_event_v = is_event<T>::value;

#endif