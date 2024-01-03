/**
 * @file types.hpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-29
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#pragma once

#include <functional>
#include <map>
#include <optional>

#include "x64/x64.hpp"

namespace kuso {

/**
 * @brief Holds information about a type
 * 
 */
struct Type {
  int                                       size{x64::Size::QWORD};
  std::optional<std::map<std::string, int>> offsets;

  [[nodiscard]] auto get_offset(const std::string& attribute) const -> int {
    if (!offsets.has_value()) return 0;
    return offsets->at(attribute);
  }
};

struct TypeID {
  size_t id;

  // NOLINTNEXTLINE
  operator size_t() const { return id; }
};

class TypeContainer {
 public:
  void add_type(const std::string& name, const Type& type) {
    _typeIDs[name] = TypeID{_types.size()};
    _types[TypeID{_types.size()}] = type;
  }

  [[nodiscard]] auto get_type_id(const std::string& name) -> std::optional<TypeID> {
    if (_typeIDs.contains(name)) return _typeIDs.at(name);
    return std::nullopt;
  }

  [[nodiscard]] auto get_type(TypeID tid) -> std::optional<std::reference_wrapper<Type>> {
    if (_types.contains(tid)) return _types.at(tid);
    return std::nullopt;
  }

  [[nodiscard]] auto get_type(const std::string& name) -> std::optional<std::reference_wrapper<Type>> {
    if (_typeIDs.contains(name)) return get_type(_typeIDs.at(name));
    return std::nullopt;
  }

 private:
  std::map<std::string, TypeID> _typeIDs;
  std::map<TypeID, Type>        _types;
};
}  // namespace kuso