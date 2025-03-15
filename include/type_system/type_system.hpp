

#pragma once

#include <cstddef>
#include <map>
#include <optional>
#include <string>

#include "type_id.hpp"
#include "types.hpp"

namespace kuso {
class TypeSystem {
 public:
  auto add_type(const std::string& name, const Type& type) -> TypeID;

  [[nodiscard]] auto get_type_id(const std::string& name) const -> TypeID;
  [[nodiscard]] auto get_type(TypeID tid) const -> Type const*;
  [[nodiscard]] auto get_type(const std::string& name) const -> Type const*;

 private:
  std::map<std::string, TypeID> _typeIDs;
  std::map<TypeID, Type>        _types;

 public:
  TypeSystem();
  TypeSystem(const TypeSystem&) = default;
  TypeSystem(TypeSystem&&) = default;
  auto operator=(const TypeSystem&) -> TypeSystem& = default;
  auto operator=(TypeSystem&&) -> TypeSystem& = default;
  ~TypeSystem() = default;
};
}  // namespace kuso
