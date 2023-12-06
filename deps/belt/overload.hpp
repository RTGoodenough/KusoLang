#pragma once

namespace belt {
template <typename... func_ts>
struct overload : func_ts... {
  using func_ts::operator()...;
};
template <class... func_ts>
overload(func_ts...) -> overload<func_ts...>;
}  // namespace belt