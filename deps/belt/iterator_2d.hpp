#pragma once

#include <span>

#include "class_macros.hpp"

namespace belt {
template <typename data_t>
class Iterator2D {
  NON_DEFAULT_CONSTRUCTIBLE(Iterator2D)
  DEFAULT_DESTRUCTIBLE(Iterator2D)
  DEFAULT_COPYABLE(Iterator2D)
  DEFAULT_MOVABLE(Iterator2D)

 public:
  using value_t = std::remove_reference_t<data_t>;
  using pointer_t = value_t*;
  using reference_t = std::conditional_t<std::is_reference_v<data_t>, data_t, data_t&>;
  using iterator_category = std::input_iterator_tag;
  using difference_type = std::ptrdiff_t;

  constexpr Iterator2D(std::span<data_t> data, std::pair<size_t, size_t> dims) noexcept
      : _data(data), _width(dims.first), _height(dims.second), _pos(0) {}

  [[nodiscard]] constexpr auto begin() -> Iterator2D { return Iterator2D(_data, {_width, _height}, 0); }
  [[nodiscard]] constexpr auto end() -> Iterator2D {
    return Iterator2D(_data, {_width, _height}, std::distance(_data.begin(), _data.end()));
  }
  [[nodiscard]] constexpr auto width() const noexcept -> size_t { return _width; }
  [[nodiscard]] constexpr auto height() const noexcept -> size_t { return _height; }
  [[nodiscard]] constexpr auto data() const noexcept -> std::span<data_t> { return _data; }

  [[nodiscard]] constexpr auto operator*() const noexcept -> std::tuple<const reference_t, size_t, size_t> {
    return {*(_data.begin() + _pos), x(), y()};
  }
  [[nodiscard]] constexpr auto operator*() noexcept -> std::tuple<reference_t, size_t, size_t> {
    return {*(_data.begin() + _pos), x(), y()};
  }

  // ----------------------------- Arithmetic Operators ------------------------------------
  auto operator+(size_t n) const noexcept = delete;
  auto operator+(const Iterator2D& rhs) const noexcept = delete;
  auto operator-(size_t n) const noexcept = delete;
  auto operator-(const Iterator2D& rhs) const noexcept = delete;
  auto operator+=(size_t n) noexcept = delete;
  auto operator+=(const Iterator2D& rhs) noexcept = delete;
  auto operator-=(size_t n) noexcept = delete;
  auto operator-=(const Iterator2D& rhs) noexcept = delete;

  // ----------------------------- Boolean Operators ------------------------------------
  [[nodiscard]] constexpr auto operator==(const Iterator2D& rhs) const noexcept -> bool {
    return (_data.data() == rhs._data.data()) && (_pos == rhs._pos);
  }
  [[nodiscard]] constexpr auto operator!=(const Iterator2D& rhs) const noexcept -> bool {
    return !((_data.data() == rhs._data.data()) && (_pos == rhs._pos));
  }
  [[nodiscard]] constexpr auto operator<(const Iterator2D& rhs) const noexcept -> bool {
    return (_data.data() == rhs._data.data()) && (_pos < rhs._pos);
  }
  [[nodiscard]] constexpr auto operator>(const Iterator2D& rhs) const noexcept -> bool {
    return (_data.data() == rhs._data.data()) && (_pos > rhs._pos);
  }
  [[nodiscard]] constexpr auto operator<=(const Iterator2D& rhs) const noexcept -> bool {
    return (_data.data() == rhs._data.data()) && (_pos <= rhs._pos);
  }
  [[nodiscard]] constexpr auto operator>=(const Iterator2D& rhs) const noexcept -> bool {
    return (_data.data() == rhs._data.data()) && (_pos >= rhs._pos);
  }

  constexpr auto operator++() noexcept -> Iterator2D& {
    ++_pos;
    return *this;
  }

  constexpr auto operator++(int) noexcept -> Iterator2D {
    auto tmp = *this;
    ++*this;
    return tmp;
  }

  [[nodiscard]] constexpr auto x() const noexcept -> size_t { return _pos % _width; }
  [[nodiscard]] constexpr auto y() const noexcept -> size_t { return _pos / _width; }

 private:
  std::span<data_t> _data;
  size_t            _width{};
  size_t            _height{};
  size_t            _pos;

  constexpr Iterator2D(std::span<data_t> data, std::pair<size_t, size_t> dims, size_t pos) noexcept
      : _data(data), _width(dims.first), _height(dims.second), _pos(pos) {}
};
}  // namespace belt