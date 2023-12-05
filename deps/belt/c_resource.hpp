#pragma once
#include <utility>
#if __cplusplus < 202002L
#error "c_resource: C++20 or higher is required"
#else

#include <concepts>
#include <type_traits>

#include "attributes.hpp"

namespace belt {

template <typename pointer_t, typename constructor_t, typename... arg_ts>
concept C_Resource_Constructable =
    std::is_invocable_r_v<pointer_t, constructor_t> || std::is_invocable_v<constructor_t, pointer_t> ||
    std::is_invocable_r_v<pointer_t, constructor_t, arg_ts...>;

template <typename pointer_t, typename destructor_t>
concept C_Resource_Destructible =
    std::is_invocable_v<destructor_t, pointer_t> || std::is_invocable_v<destructor_t, pointer_t*>;

template <typename data_t, auto* constructor, auto* destructor>
class C_Resource {
  using constructor_t = decltype(constructor);
  using destructor_t = decltype(destructor);
  using pointer_t = std::decay_t<data_t>*;

  static_assert(std::is_function_v<std::remove_pointer_t<constructor_t>>,
                "Constructor Needs To Be A Function");
  static_assert(std::is_function_v<std::remove_pointer_t<destructor_t>>, "Destructor Needs To Be A Function");

 public:
  // Constructors
  constexpr C_Resource() noexcept = default;
  constexpr C_Resource() requires C_Resource_Constructable<pointer_t, constructor_t> { construct(); }

  // Destructors
  constexpr ~C_Resource() noexcept { destroy(); }

  // Parameterized Constructors
  template <typename... arg_ts>
  explicit constexpr C_Resource(arg_ts&&... args) noexcept requires
      C_Resource_Constructable<data_t, pointer_t, constructor_t, arg_ts...> {
    construct(std::forward<arg_ts>(args)...);
  }

  // TODO(rolland): figure out copying
  // Copy Constructors
  constexpr C_Resource(const C_Resource& other) noexcept = delete;
  constexpr auto operator=(const C_Resource& rhs) noexcept -> C_Resource& = delete;

  // Move Constructors
  constexpr C_Resource(C_Resource&& other) noexcept : _ptr{other._ptr} { other._ptr = nullptr; };
  constexpr auto operator=(C_Resource&& rhs) noexcept
      -> C_Resource& requires C_Resource_Destructible<pointer_t, destructor_t> {
    [[likely]] if (this != &rhs) {
      destroy();
      _ptr = rhs._ptr;
      rhs._ptr = nullptr;
    }
    return *this;
  };

  /**
   * @brief Returns whether the internal pointer is null
   * 
   * @return true : Internal pointer is null
   * @return false : Internal pointer is not null
   */
  constexpr auto     has_value() noexcept -> bool { return _ptr != nullptr; }
  constexpr explicit operator bool() const noexcept { return _ptr != nullptr; }

  /**
   * @brief Returns a reference to the internal pointer to the resource
   * 
   * @return pointer_t : Pointer to the resource
   */
  WARN_UNSAFE constexpr auto     raw() noexcept -> pointer_t& { return _ptr; }
  WARN_UNSAFE constexpr explicit operator pointer_t() noexcept { return _ptr; }
  constexpr auto                 operator->() noexcept -> pointer_t { return _ptr; }

  /**
   * @brief Destroys the resource and updates the pointer with the provided one
   * 
   * @param ptr : Pointer to the new resource
   */
  constexpr void reset(pointer_t ptr) noexcept {
    destroy();
    _ptr = ptr;
  }

  /**
   * @brief Returns the pointer to the resource and sets the internal pointer to null
   * 
   * @return pointer_t : Pointer to the resource
   */
  constexpr auto release() -> pointer_t {
    auto ptr = _ptr;
    _ptr = nullptr;
    return ptr;
  }

  /**
   * @brief Swaps resource pointers
   * 
   * @param other : Resource to swap with
   */
  constexpr void swap(C_Resource& other) noexcept {
    auto ptr = _ptr;
    _ptr = other._ptr;
    other._ptr = ptr;
  }

 private:
  pointer_t _ptr = nullptr;

  /**
   * @brief Finds the appropriate destructor and destroys the resource with it
   * 
   */
  inline constexpr void destroy() noexcept {
    [[likely]] if (_ptr) {
      if constexpr (std::is_invocable_v<destructor_t, pointer_t>) {
        DESTRUCTOR(_ptr);
      } else if constexpr (std::is_invocable_v<destructor_t, pointer_t*>) {
        DESTRUCTOR(&_ptr);
      } else {
        static_assert(
            (std::is_invocable_v<destructor_t, pointer_t> || std::is_invocable_v<destructor_t, pointer_t*>),
            "Destructor Cannot Be Called with Data Pointer");
      }
    }
  }

  /**
   * @brief Finds the appropriate constructor and constructs the resource with it
   * 
   * @tparam arg_ts : Constructor Argument Types
   * @param args : Argument values
   */
  template <typename... arg_ts>
  inline constexpr void construct(arg_ts&&... args) noexcept {
    if constexpr (std::is_invocable_r_v<pointer_t, constructor_t>) {
      _ptr = CONSTRUCTOR();
    } else if constexpr (std::is_invocable_v<constructor_t, pointer_t>) {
      CONSTRUCTOR(_ptr);
    } else if constexpr (std::is_invocable_r_v<pointer_t, constructor_t, arg_ts...>) {
      _ptr = CONSTRUCTOR(std::forward<arg_ts>(args)...);
    } else {
      static_assert(
          (std::is_invocable_r_v<pointer_t, constructor_t> || std::is_invocable_v<constructor_t, pointer_t> ||
           std::is_invocable_r_v<pointer_t, constructor_t, arg_ts...>),
          "Constructor Cannot Be Called with Data Pointer");
    }
  }

 public:
  static constexpr constructor_t CONSTRUCTOR = constructor;
  static constexpr destructor_t  DESTRUCTOR = destructor;
};

}  // namespace belt
#endif