#pragma once

#include <coroutine>
#include <memory>

#include "class_macros.hpp"

namespace belt {

template <typename data_t>
class Promise;
template <typename data_t, typename sentinel_t>
class CoroIterator;

template <typename data_t>
class Generator {
  NON_DEFAULT_DEFAULT_CONSTRUCTIBLE(Generator)
  COPYABLE(Generator)
  MOVABLE(Generator)

 public:
  struct Sentinel {};
  using sentinel = Sentinel;
  using promise_type = Promise<data_t>;
  using iterator = CoroIterator<data_t, sentinel>;
  using handle_t = std::coroutine_handle<promise_type>;

  constexpr auto begin() noexcept -> iterator {
    if (_handle) {
      _handle.resume();
    }
    return iterator(_handle);
  }

  constexpr auto end() noexcept -> sentinel { return {}; }

 private:
  handle_t _handle;

 public:
  constexpr explicit Generator(handle_t handle) noexcept : _handle(handle) {}

  constexpr ~Generator() noexcept {
    if (_handle) {
      _handle.destroy();
    }
  }
};

template <typename data_t>
class Promise {
  DEFAULT_CONSTRUCTIBLE(Promise)
  DEFAULT_DESTRUCTIBLE(Promise)
  DEFAULT_COPYABLE(Promise)
  DEFAULT_MOVABLE(Promise)

 public:
  using value_t = std::remove_reference_t<data_t>;
  using reference_t = std::conditional_t<std::is_reference_v<data_t>, data_t, data_t&>;
  using pointer_t = value_t*;

  [[nodiscard]] constexpr auto get_return_object() noexcept -> Generator<data_t> {
    return Generator<data_t>{std::coroutine_handle<Promise<data_t>>::from_promise(*this)};
  }

  [[nodiscard]] constexpr auto final_suspend() noexcept -> std::suspend_always { return {}; }
  [[nodiscard]] constexpr auto initial_suspend() noexcept -> std::suspend_always { return {}; }

  [[nodiscard]] constexpr auto yield_value(std::remove_reference_t<data_t>& value) noexcept
      -> std::suspend_always {
    _value = std::addressof(value);
    return {};
  }
  [[nodiscard]] constexpr auto yield_value(std::remove_reference_t<data_t>&& value) noexcept
      -> std::suspend_always {
    _value = std::addressof(value);
    return {};
  }
  constexpr auto return_value(std::remove_reference_t<data_t>& value) noexcept -> std::suspend_never {
    _value = std::addressof(value);
    return {};
  }
  constexpr auto return_value(std::remove_reference_t<data_t>&& value) noexcept -> std::suspend_never {
    _value = std::addressof(value);
    return {};
  }

  void unhandled_exception() noexcept { std::terminate(); }

  [[nodiscard]] constexpr auto value() const noexcept -> reference_t {
    return static_cast<reference_t>(*_value);
  }

 private:
  pointer_t _value;
};

template <typename data_t, typename sentinel_t>
class CoroIterator {
  DEFAULT_CONSTRUCTIBLE(CoroIterator)
  DEFAULT_DESTRUCTIBLE(CoroIterator)
  DEFAULT_COPYABLE(CoroIterator)
  DEFAULT_MOVABLE(CoroIterator)

 public:
  using iterator_category = std::input_iterator_tag;
  using value_type = data_t;
  using difference_type = std::ptrdiff_t;
  using pointer = data_t*;
  using reference = data_t&;

  constexpr explicit CoroIterator(std::coroutine_handle<Promise<data_t>> handle) noexcept : _handle(handle) {}

  constexpr auto operator++() noexcept -> CoroIterator& {
    _handle.resume();
    return *this;
  }

  constexpr auto operator++(int) noexcept -> CoroIterator {
    auto copy = *this;
    ++(*this);
    return copy;
  }

  [[nodiscard]] constexpr auto operator*() noexcept -> data_t& { return _handle.promise().value(); }

  [[nodiscard]] constexpr auto operator==(sentinel_t /*unused*/) noexcept -> bool { return _handle.done(); }
  [[nodiscard]] constexpr auto operator==(const CoroIterator& iter) noexcept -> bool {
    return iter._handle.done();
  }
  [[nodiscard]] constexpr auto operator!=(sentinel_t /*unused*/) noexcept -> bool { return !_handle.done(); }
  [[nodiscard]] constexpr auto operator!=(const CoroIterator& iter) noexcept -> bool {
    return !iter._handle.done();
  }

 private:
  std::coroutine_handle<Promise<data_t>> _handle;
};
}  // namespace belt