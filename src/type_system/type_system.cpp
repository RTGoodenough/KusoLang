
#include "type_system/type_system.hpp"
#include "type_system/type_id.hpp"

#include "exceptions/type_exception.hpp"

#include "x64/x64.hpp"

namespace kuso {
auto TypeSystem::add_type(const std::string& name, const Type& type) -> TypeID
{
  if ( _typeIDs.contains(name) )
    throw TypeException("Type Already Exists with Name: " + name);

  _typeIDs[name] = _types.size();
  _types[_types.size()] = type;
  return _types.size() - 1;
}

auto TypeSystem::get_type_id(const std::string& name) const -> TypeID
{
  if ( _typeIDs.contains(name) ) return _typeIDs.at(name);

  throw TypeException("No Type with Name: " + name);
}

auto TypeSystem::get_type(TypeID tid) const -> Type const*
{
  if ( _types.contains(tid) ) return &_types.at(tid);

  throw TypeException("No Type with ID: " + std::to_string(tid));
}

auto TypeSystem::get_type(const std::string& name) const -> Type const*
{
  if ( _typeIDs.contains(name) ) return get_type(_typeIDs.at(name));

  throw TypeException("No Type with Name: " + name);
}

TypeSystem::TypeSystem()
{
  add_type("none", {0, 0, 0, "none"});
  add_type("int", {0, x64::Size::WORD, x64::Size::WORD, "int"});
  add_type("char", {0, x64::Size::BYTE, x64::Size::BYTE, "char"});
  add_type("flt", {0, x64::Size::WORD, x64::Size::WORD, "flt"});
  add_type("dbl", {0, x64::Size::QWORD, x64::Size::QWORD, "dbl"});
}
}  // namespace kuso