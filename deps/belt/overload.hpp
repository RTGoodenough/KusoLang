#pragma once

#include <type_traits>
#include <utility>
#include <variant>

namespace belt {
template <typename... func_ts>
struct overload : func_ts... {
  using func_ts::operator()...;
};
template <class... func_ts>
overload(func_ts...) -> overload<func_ts...>;

template <typename data_t, typename... func_ts>
auto overloaded_visit(data_t&& obj, func_ts&&... funcs) {
  return std::visit(overload<std::decay_t<func_ts>...>(std::forward<func_ts>(funcs)...), obj);
}
}  // namespace belt