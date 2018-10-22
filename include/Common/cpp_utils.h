#ifndef CPP_UTILS_H
#define CPP_UTILS_H

#include <type_traits>

template< class... >
using void_t = void;

#define HAS_METHOD(method)\
template <class T, class=void>\
struct _has_##method : std::false_type{};\
\
template <class T>\
struct _has_##method<T, void_t<std::is_member_function_pointer<decltype(&T::method)>>>\
       : std::true_type{};\
\
template< class T>\
static inline constexpr bool has_##method = _has_##method<T>::value;


#endif // CPP_UTILS_H
