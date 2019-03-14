#ifndef VARIANTWITHBASE_H
#define VARIANTWITHBASE_H
// stolen without any shame from
// https://tower120.github.io/2018/05/18/variant_with_base.html (•_•) (
// •_•)>⌐■-■ (⌐■_■)
#include <cassert>
#include <tuple>
#include <type_traits>
#include <variant>

template<class Base, class Variant> class variant_w_base
{
  using Self = variant_w_base<Base, Variant>;

  auto& self_mut() const { return *const_cast<Self*>(this); }

  Base* m_base;
  Variant m_variant;

  void update_base()
  {
    m_base = std::visit(
        [](auto&& arg) -> Base* {
          using Arg = std::decay_t<decltype(arg)>;
          if constexpr(std::is_same_v<Arg, std::monostate>) { return nullptr; }
          else
          {
            return static_cast<Base*>(&arg);
          }
        },
        m_variant);
  }
  template<class T> void update_base()
  {
    using Arg = std::decay_t<T>;
    if constexpr(std::is_same_v<Arg, std::monostate>) { m_base = nullptr; }
    else
    {
      m_base = std::get_if<Arg>(&m_variant);
      assert(m_base);
    }
  }

  template<class T>
  using is_not_self = std::enable_if_t<!std::is_same_v<std::decay_t<T>, Self>>;

public:
  variant_w_base() { update_base(); }
  variant_w_base(const variant_w_base& other) : m_variant(other.m_variant)
  {
    update_base();
  }
  variant_w_base(variant_w_base&& other) : m_variant(std::move(other.m_variant))
  {
    update_base();
  }
  variant_w_base(const Variant& var) : m_variant(var) { update_base(); }
  variant_w_base(Variant&& var) : m_variant(std::move(var)) { update_base(); }
  template<class T, typename = is_not_self<T>>
  variant_w_base(T&& value) : m_variant(std::forward<T>(value))
  {
    update_base<T>();
  }
  template<class T, class... Args>
  explicit variant_w_base(std::in_place_type_t<T>, Args&&... args)
      : m_variant(std::in_place_type_t<T>(), std::forward<Args>(args)...)
  {
    update_base<T>();
  }

  variant_w_base& operator=(const variant_w_base& other)
  {
    m_variant = other.m_variant;
    update_base();
    return *this;
  }
  variant_w_base& operator=(variant_w_base&& other)
  {
    m_variant = std::move(other.m_variant);
    update_base();
    return *this;
  }

  template<class T, typename = is_not_self<T>>
  variant_w_base& operator=(T&& value)
  {
    m_variant = std::forward<T>(value);
    update_base<T>();
    return *this;
  }
  variant_w_base& operator=(const Variant& var)
  {
    m_variant = var;
    update_base();
    return *this;
  }
  variant_w_base& operator=(Variant&& var)
  {
    m_variant = std::move(var);
    update_base();
    return *this;
  }
  constexpr std::size_t index() const noexcept { return m_variant.index(); }
  constexpr bool operator==(const variant_w_base& other) const
  {
    return m_variant == other.m_variant;
  }
  constexpr bool operator!=(const variant_w_base& other) const
  {
    return m_variant != other.m_variant;
  }

  // free functions from std::variant
  template<class T> constexpr T* get_if()
  {
    if constexpr(std::is_same_v<T, Base>) { return base(); }
    else
    {
      return std::get_if<T>(variant());
    }
  }
  template<class T> constexpr const T* get_if() const
  {
    return self_mut().template get<T>();
  }

  template<std::size_t I> constexpr decltype(auto) get_if()
  {
    return std::get_if<I>(variant());
  }
  template<std::size_t I> constexpr decltype(auto) get_if() const
  {
    return std::get_if<I>(variant());
  }

  template<class T> constexpr T& get()
  {
    if constexpr(std::is_same_v<T, Base>)
    {
      if(base() == nullptr) { throw std::bad_variant_access(); }
      return *base();
    }
    else
    {
      return std::get<T>(variant());
    }
  }
  template<class T> constexpr const T& get() const
  {
    return self_mut().template get<T>();
  }

  template<std::size_t I> constexpr decltype(auto) get()
  {
    return std::get<I>(variant());
  }
  template<std::size_t I> constexpr decltype(auto) get() const
  {
    return std::get<I>(variant());
  }

  template<class Visitor> constexpr decltype(auto) visit(Visitor&& vis)
  {
    return std::visit(std::forward<Visitor>(vis), variant());
  }
  template<class Visitor> constexpr decltype(auto) visit(Visitor&& vis) const
  {
    return std::visit(std::forward<Visitor>(vis), variant());
  }

  Base* base() noexcept { return m_base; }
  const Base* base() const noexcept { return m_base; }
  operator Base&() noexcept { return *m_base; }
  operator const Base&() const noexcept { return *m_base; }
  Base* operator->() noexcept { return m_base; }
  const Base* operator->() const noexcept { return m_base; }
  Base& operator*() noexcept { return m_base; }
  const Base& operator*() const noexcept { return m_base; }

  constexpr const Variant& variant() const noexcept { return m_variant; }

private:
  // hide, to keep variant type change tracked.
  constexpr Variant& variant() noexcept { return m_variant; }
};

#endif // VARIANTWITHBASE_H
