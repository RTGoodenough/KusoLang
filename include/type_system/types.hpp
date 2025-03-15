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
#include <string>
#include <vector>

#include "exceptions/type_exception.hpp"

#include "type_id.hpp"

#include "transforms.hpp"

namespace kuso {

class TypeSystem;

/**
 * @brief Holds information about a type
 * 
 */
struct Type {
  TypeID id;
  size_t size;
  size_t alignment;

  std::string                   name;
  std::map<std::string, size_t> attributes{};
  std::vector<Type const*>      subtypes{};
  std::vector<size_t>           offsets{};
  std::vector<Transform const*> transforms{};

  void add_attribute(std::string const& name, std::string const& type,
                     TypeSystem const& system);

  void add_transform(std::string const& name, TypeSystem const& system);
  auto get_transforms(std::string const& name, TypeSystem const& system)
      -> std::vector<Transform> const&;

  [[nodiscard]] auto get_offset(std::string const& attribute) const -> size_t;
  [[nodiscard]] auto get_attribute_type(std::string const& attribute) const
      -> Type const*;
};

}  // namespace kuso