/**
 * @file x64.hpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-19
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#pragma once

#include <stdexcept>
#include <string>

namespace kuso::x64 {
/**
  * @brief x86_64 word sizes
  * 
  */
enum Size { BYTE = 1, WORD = 2, DWORD = 4, QWORD = 8 };

/**
 * @brief x86_64 Register count
 * 
 */
constexpr size_t REGISTER_COUNT = 15;

/**
 * @brief x86_64 Registers 
 * 
 */
enum class Register {
  RAX = 0,
  RBX = 1,
  RCX = 2,
  RDX = 3,
  RSI = 4,
  RDI = 5,
  RBP = 6,
  R8 = 7,
  R9 = 8,
  R10 = 9,
  R11 = 10,
  R12 = 11,
  R13 = 12,
  R14 = 13,
  R15 = 14,
  RSP,
  NONE,
  EAX,
  EBX,
  ECX,
  EDX,
  ESI,
  EDI,
  ESP,
  EBP,
  R8D,
  R9D,
  R10D,
  R11D,
  R12D,
  R13D,
  R14D,
  R15D,
  AX,
  BX,
  CX,
  DX,
  SI,
  DI,
  SP,
  BP,
  R8W,
  R9W,
  R10W,
  R11W,
  R12W,
  R13W,
  R14W,
  R15W,
  AL,
  BL,
  CL,
  DL,
  SIL,
  DIL,
  SPL,
  BPL,
  R8B,
  R9B,
  R10B,
  R11B,
  R12B,
  R13B,
  R14B,
  R15B
};

/**
 * @brief x86_64 Operations
 * 
 */
enum class Op {
  MOV,
  MOVL,
  ADD,
  SUB,
  MUL,
  IMUL,
  DIV,
  IDIV,
  MOD,
  AND,
  OR,
  XOR,
  NOT,
  NEG,
  SHL,
  SHR,
  CMP,
  JMP,
  JE,
  JNE,
  JG,
  JGE,
  JL,
  JLE,
  CALL,
  RET,
  PUSH,
  POP,
  LEA,
  NOP,
  MOVZX,
  INT,
  SETGE,
  SETE,
  SETNE,
  SETG,
  SETL,
  SETLE,
  SYSCALL,
  ENTER,
  LEAVE,
};

struct Literal {
  int64_t   value{};
  x64::Size size{x64::Size::DWORD};

  [[nodiscard]] auto to_string() const -> std::string { return std::to_string(value); }
  // NOLINTNEXTLINE(google-explicit-constructor)
  [[nodiscard]] operator int64_t() const { return value; }
};

[[nodiscard]] inline auto to_string(Size size) -> std::string {
  switch (size) {
    case Size::BYTE:
      return "byte";
    case Size::WORD:
      return "word";
    case Size::DWORD:
      return "dword";
    case Size::QWORD:
      return "qword";
  }
  throw std::runtime_error("Invalid Size");
}

[[nodiscard]] inline auto parameter_reg(size_t param) -> Register {
  switch (param) {
    case 0:
      return Register::RDI;
    case 1:
      return Register::RSI;
    case 2:
      return Register::RDX;
    case 3:
      return Register::RCX;
    case 4:
      return Register::R8;
    case 5:
      return Register::R9;
    default:
      break;
  }

  return Register::NONE;
}

[[nodiscard]] inline auto return_reg(size_t param) -> Register {
  switch (param) {
    case 0:
      return Register::RAX;
    case 1:
      return Register::RDX;
    default:
      break;
  }
  throw std::runtime_error("Invalid Parameter");
}

[[nodiscard]] inline auto to_string(Register reg) -> std::string {
  switch (reg) {
    case Register::RAX:
      return "rax";
    case Register::RBX:
      return "rbx";
    case Register::RCX:
      return "rcx";
    case Register::RDX:
      return "rdx";
    case Register::RSI:
      return "rsi";
    case Register::RDI:
      return "rdi";
    case Register::RSP:
      return "rsp";
    case Register::RBP:
      return "rbp";
    case Register::R8:
      return "r8";
    case Register::R9:
      return "r9";
    case Register::R10:
      return "r10";
    case Register::R11:
      return "r11";
    case Register::R12:
      return "r12";
    case Register::R13:
      return "r13";
    case Register::R14:
      return "r14";
    case Register::R15:
      return "r15";
    case Register::NONE:
      break;
    case Register::EAX:
      return "eax";
    case Register::EBX:
      return "ebx";
    case Register::ECX:
      return "ecx";
    case Register::EDX:
      return "edx";
    case Register::ESI:
      return "esi";
    case Register::EDI:
      return "edi";
    case Register::ESP:
      return "esp";
    case Register::EBP:
      return "ebp";
    case Register::R8D:
      return "r8d";
    case Register::R9D:
      return "r9d";
    case Register::R10D:
      return "r10d";
    case Register::R11D:
      return "r11d";
    case Register::R12D:
      return "r12d";
    case Register::R13D:
      return "r13d";
    case Register::R14D:
      return "r14d";
    case Register::R15D:
      return "r15d";
    case Register::AX:
      return "ax";
    case Register::BX:
      return "bx";
    case Register::CX:
      return "cx";
    case Register::DX:
      return "dx";
    case Register::SI:
      return "si";
    case Register::DI:
      return "di";
    case Register::SP:
      return "sp";
    case Register::BP:
      return "bp";
    case Register::R8W:
      return "r8w";
    case Register::R9W:
      return "r9w";
    case Register::R10W:
      return "r10w";
    case Register::R11W:
      return "r11w";
    case Register::R12W:
      return "r12w";
    case Register::R13W:
      return "r13w";
    case Register::R14W:
      return "r14w";
    case Register::R15W:
      return "r15w";
    case Register::AL:
      return "al";
    case Register::BL:
      return "bl";
    case Register::CL:
      return "cl";
    case Register::DL:
      return "dl";
    case Register::SIL:
      return "sil";
    case Register::DIL:
      return "dil";
    case Register::SPL:
      return "spl";
    case Register::BPL:
      return "bpl";
    case Register::R8B:
      return "r8b";
    case Register::R9B:
      return "r9b";
    case Register::R10B:
      return "r10b";
    case Register::R11B:
      return "r11b";
    case Register::R12B:
      return "r12b";
    case Register::R13B:
      return "r13b";
    case Register::R14B:
      return "r14b";
    case Register::R15B:
      return "r15b";
    default:
      break;
  }
  throw std::runtime_error("Invalid Register");
}

[[nodiscard]] inline auto to_string(Op operation) -> std::string {
  switch (operation) {
    case Op::MOV:
      return "mov";
    case Op::MOVL:
      return "movl";
    case Op::ADD:
      return "add";
    case Op::SUB:
      return "sub";
    case Op::MUL:
      return "mul";
    case Op::IMUL:
      return "imul";
    case Op::DIV:
      return "div";
    case Op::IDIV:
      return "idiv";
    case Op::MOD:
      return "mod";
    case Op::AND:
      return "and";
    case Op::OR:
      return "or";
    case Op::XOR:
      return "xor";
    case Op::NOT:
      return "not";
    case Op::NEG:
      return "neg";
    case Op::SHL:
      return "shl";
    case Op::SHR:
      return "shr";
    case Op::CMP:
      return "cmp";
    case Op::JMP:
      return "jmp";
    case Op::JE:
      return "je";
    case Op::JNE:
      return "jne";
    case Op::JG:
      return "jg";
    case Op::JGE:
      return "jge";
    case Op::JL:
      return "jl";
    case Op::JLE:
      return "jle";
    case Op::CALL:
      return "call";
    case Op::RET:
      return "ret";
    case Op::PUSH:
      return "push";
    case Op::POP:
      return "pop";
    case Op::LEA:
      return "lea";
    case Op::NOP:
      return "nop";
    case Op::INT:
      return "int";
    case Op::SETGE:
      return "setge";
    case Op::SETE:
      return "sete";
    case Op::SETNE:
      return "setne";
    case Op::SETG:
      return "setg";
    case Op::SETL:
      return "setl";
    case Op::SETLE:
      return "setle";
    case Op::MOVZX:
      return "movzx";
    case Op::SYSCALL:
      return "syscall";
    case Op::ENTER:
      return "enter";
    case Op::LEAVE:
      return "leave";
  }
  throw std::runtime_error("Invalid Op");
}
}  // namespace kuso::x64