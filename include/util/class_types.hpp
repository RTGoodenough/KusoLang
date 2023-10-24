#ifndef KUSO_CLASS_TYPES_HPP
#define KUSO_CLASS_TYPES_HPP

#define DEFAULT_CONSTRUCTIBLE(T) \
 public:                         \
  T() = default;

#define COPYABLE(T)      \
 public:                 \
  T(const T&) = default; \
  auto operator=(const T&)->T& = default;

#define MOVABLE(T)  \
 public:            \
  T(T&&) = default; \
  auto operator=(T&&)->T& = default;

#define NON_COPYABLE(T) \
 public:                \
  T(const T&) = delete; \
  auto operator=(const T&)->T& = delete;

#define NON_MOVABLE(T) \
 public:               \
  T(T&&) = delete;     \
  auto operator=(T&&)->T& = delete;

#define NON_DEFAULT_CONSTRUCTIBLE(T) \
 public:                             \
  T() = delete;

#define DEFAULT_DESTRUCTIBLE(T) \
 public:                        \
  ~T() = default;

#endif