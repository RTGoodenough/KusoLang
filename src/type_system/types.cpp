

#include "exceptions/type_exception.hpp"

#include "type_system/type_system.hpp"
#include "type_system/types.hpp"

namespace kuso {
void Type::add_attribute(std::string const& name, std::string const& type,
                         TypeSystem const& system)
{
  if ( attributes.contains(name) )
    throw TypeException("Type: " + name + " has Duplicate Attribute: " + type);

  auto        attrTypeID = system.get_type_id(type);
  auto const* attrType = system.get_type(attrTypeID);

  attributes[name] = subtypes.size();
  subtypes.push_back(attrType);

  size_t offset = (offsets.back() + subtypes.back()->size + alignment - 1) &
                  ~(alignment - 1);
  offsets.push_back(offset);
}

auto Type::get_offset(std::string const& attribute) const -> size_t
{
  auto const iter = attributes.find(attribute);
  if ( iter == attributes.end() )
    throw TypeException{"Invalid Type Attribute: " + attribute +
                        " For Type: " + name};

  return offsets[(*iter).second];
}

auto Type::get_attribute_type(std::string const& attribute) const -> Type const*
{
  auto const iter = attributes.find(attribute);
  if ( iter == attributes.end() )
    throw TypeException{"Invalid Type Attribute: " + attribute +
                        " For Type: " + name};

  return subtypes[(*iter).second];
}
}  // namespace kuso