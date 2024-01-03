#ifndef KUSO_COMPILE_EXCEPTION_HPP
#define KUSO_COMPILE_EXCEPTION_HPP

#include <exception>
#include <string>

namespace kuso {
class CompileException : public std::exception {
 public:
  explicit CompileException(std::string message) : _message(std::move(message)) {}

  static void error(const std::string& message) { throw CompileException(message); }

  [[nodiscard]] auto what() const noexcept -> const char* override { return _message.c_str(); }

 private:
  std::string _message;
};
}  // namespace kuso

#endif