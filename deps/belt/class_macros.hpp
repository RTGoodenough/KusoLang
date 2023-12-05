#pragma once

#define DEFAULT_CONSTRUCTIBLE(T) \
 public:                         \
  T() = default;

#define DEFAULT_DESTRUCTIBLE(T) \
 public:                        \
  ~T() = default;

#define DEFAULT_COPYABLE(T) \
 public:                    \
  T(const T&) = default;    \
  auto operator=(const T&)->T& = default;

#define DEFAULT_MOVABLE(T)   \
 public:                     \
  T(T&&) noexcept = default; \
  auto operator=(T&&) noexcept->T& = default;

#define MOVABLE(T) \
 public:           \
  T(T&&) noexcept; \
  auto operator=(T&&) noexcept->T&;

#define COPYABLE(T) \
 public:            \
  T(const T&);      \
  auto operator=(const T&)->T&;

#define NON_DEFAULT_DEFAULT_CONSTRUCTIBLE(T) \
 public:                                     \
  T();

#define NON_DEFAULT_DEFAULT_DESTRUCTIBLE(T) \
 public:                                    \
  ~T();

#define NON_DEFAULT_CONSTRUCTIBLE(T) \
 public:                             \
  T() = delete;

#define NON_DEFAULT_COPYABLE(T) \
 public:                        \
  T(const T&);                  \
  auto operator=(const T&)->T&;

#define NON_DEFAULT_MOVABLE(T) \
 public:                       \
  T(T&&) noexcept;             \
  auto operator=(T&&) noexcept->T&;

#define NON_MOVABLE(T) \
 public:               \
  T(T&&) = delete;     \
  auto operator=(T&&)->T& = delete;

#define NON_COPYABLE(T) \
 public:                \
  T(const T&) = delete; \
  auto operator=(const T&)->T& = delete;

#define SINGLETON(T) \
 public:             \
  T() = delete;      \
  ~T() = delete;     \
  NON_COPYABLE(T)    \
  NON_MOVABLE(T);

#define VIRTUAL(T) \
 public:           \
  T() = default;   \
  virtual ~T() = default;
