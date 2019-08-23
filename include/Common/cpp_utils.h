#ifndef CPP_UTILS_H
#define CPP_UTILS_H

#include "types_detectors.h"

#include <functional>
#include <tuple>
#include <type_traits>

template<class...> using void_t = void;

#define HAS_METHOD(method)                                                     \
  template<class T, class = void> struct _has_##method : std::false_type       \
  {};                                                                          \
                                                                               \
  template<class T>                                                            \
  struct _has_##method<                                                        \
      T, void_t<std::is_member_function_pointer<decltype(&T::method)>>>        \
      : std::true_type                                                         \
  {};                                                                          \
                                                                               \
  template<class T>                                                            \
  static inline constexpr bool has_##method = _has_##method<T>::value;

// taken from here https://www.fluentcpp.com/2017/10/27/function-aliases-cpp/
#define ALIAS_TEMPLATE_FUNCTION(highLevelF, lowLevelF)                         \
  template<typename... Args>                                                   \
  inline auto highLevelF(Args&&... args)                                       \
      ->decltype(lowLevelF(std::forward<Args>(args)...))                       \
  {                                                                            \
    return lowLevelF(std::forward<Args>(args)...);                             \
  }

template<typename T> constexpr T diff(const std::pair<T, T>& p)
{
  return p.second - p.first;
}

template<typename T> constexpr auto const& to_value(const T& item)
{
  if constexpr(std::is_pointer_v<std::remove_reference_t<std::remove_cv_t<T>>>)
  { return *item; }
  else
  {
    if constexpr(is_smart_ptr<T>::value) { return *item.get(); }
    else
    {
      return item;
    }
  }
}

template<typename T> void repeat_n(T func, int number)
{
  for(int i = 0; i < number; i++)
    func();
}

inline int operator*(int number, const std::function<void(void)>& func)
{
  for(int i = 0; i < number; i++)
    func();
  return number;
}

#endif // CPP_UTILS_H
