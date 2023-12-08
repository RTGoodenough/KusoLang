#pragma once

#include <stdexcept>
#include <string>

namespace kuso::x86 {
enum class Register { RAX, RBX, RCX, RDX, RSI, RDI, RSP, RBP, R8, R9, R10, R11, R12, R13, R14, R15 };

struct Address {
  int offset;
};

struct Source {
  Register reg;
  // NOLINTNEXTLINE(google-explicit-constructor)
  operator Register() const { return reg; }
};
struct Destination {
  Register reg;
  // NOLINTNEXTLINE(google-explicit-constructor)
  operator Register() const { return reg; }
};

enum class Op {
  MOV,
  ADD,
  SUB,
  MUL,
  DIV,
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
  INT,
  SYSCALL,
  SYSENTER,
  SYSEXIT,
  SYSEXITQ,
  SYSCALLQ,
  SYSENTERQ,
  SYSEXITL,
  SYSCALLL,
  SYSENTERL,
  SYSEXITW,
  SYSCALLW,
  SYSENTERW,
  SYSEXITB,
  SYSCALLB,
  SYSENTERB,
  SYSEXITD,
  SYSCALLD,
  SYSENTERD,
  SYSEXITF,
  SYSCALLF,
  SYSENTERF,
  SYSEXITP,
  SYSCALLP,
  SYSENTERP,
  SYSEXITWQ,
  SYSCALLWQ,
  SYSENTERWQ,
  SYSEXITBQ,
  SYSCALLBQ,
  SYSENTERBQ,
  SYSEXITDQ,
  SYSCALLDQ,
  SYSENTERDQ,
  SYSEXITFQ,
  SYSCALLFQ,
  SYSENTERFQ,
  SYSEXITPQ,
  SYSCALLPQ,
  SYSENTERPQ,
  SYSEXITWL,
  SYSCALLWL,
  SYSENTERWL,
  SYSEXITBL,
  SYSCALLBL,
  SYSENTERBL,
  SYSEXITDL,
  SYSCALLDL,
  SYSENTERDL,
  SYSEXITFL,
  SYSCALLFL,
  SYSENTERFL,
  SYSEXITPL,
  SYSCALLPL,
  SYSENTERPL,
  SYSEXITWW,
  SYSCALLWW,
  SYSENTERWW,
  SYSEXITBW,
  SYSCALLBW,
  SYSENTERBW,
  SYSEXITDW,
  SYSCALLDW,
  SYSENTERDW,
  SYSEXITFW,
  SYSCALLFW,
  SYSENTERFW,
  SYSEXITPW,
  SYSCALLPW,
  SYSENTERPW,
};

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
  }
  throw std::runtime_error("Invalid Register");
}

[[nodiscard]] inline auto to_string(Op operation) -> std::string {
  switch (operation) {
    case Op::MOV:
      return "mov";
    case Op::ADD:
      return "add";
    case Op::SUB:
      return "sub";
    case Op::MUL:
      return "mul";
    case Op::DIV:
      return "div";
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
    case Op::SYSCALL:
      return "syscall";
    case Op::SYSENTER:
      return "sysenter";
    case Op::SYSEXIT:
      return "sysexit";
    case Op::SYSEXITQ:
      return "sysexitq";
    case Op::SYSCALLQ:
      return "syscallq";
    case Op::SYSENTERQ:
      return "sysenterq";
    case Op::SYSEXITL:
      return "sysexitl";
    case Op::SYSCALLL:
      return "syscalll";
    case Op::SYSENTERL:
      return "sysenterl";
    case Op::SYSEXITW:
      return "sysexitw";
    case Op::SYSCALLW:
      return "syscallw";
    case Op::SYSENTERW:
    case Op::SYSEXITB:
    case Op::SYSCALLB:
    case Op::SYSENTERB:
    case Op::SYSEXITD:
    case Op::SYSCALLD:
    case Op::SYSENTERD:
    case Op::SYSEXITF:
    case Op::SYSCALLF:
    case Op::SYSENTERF:
    case Op::SYSEXITP:
    case Op::SYSCALLP:
    case Op::SYSENTERP:
    case Op::SYSEXITWQ:
    case Op::SYSCALLWQ:
    case Op::SYSENTERWQ:
    case Op::SYSEXITBQ:
    case Op::SYSCALLBQ:
    case Op::SYSENTERBQ:
    case Op::SYSEXITDQ:
    case Op::SYSCALLDQ:
    case Op::SYSENTERDQ:
    case Op::SYSEXITFQ:
    case Op::SYSCALLFQ:
    case Op::SYSENTERFQ:
    case Op::SYSEXITPQ:
    case Op::SYSCALLPQ:
    case Op::SYSENTERPQ:
    case Op::SYSEXITWL:
    case Op::SYSCALLWL:
    case Op::SYSENTERWL:
    case Op::SYSEXITBL:
    case Op::SYSCALLBL:
    case Op::SYSENTERBL:
    case Op::SYSEXITDL:
    case Op::SYSCALLDL:
    case Op::SYSENTERDL:
    case Op::SYSEXITFL:
    case Op::SYSCALLFL:
    case Op::SYSENTERFL:
    case Op::SYSEXITPL:
    case Op::SYSCALLPL:
    case Op::SYSENTERPL:
    case Op::SYSEXITWW:
    case Op::SYSCALLWW:
    case Op::SYSENTERWW:
    case Op::SYSEXITBW:
    case Op::SYSCALLBW:
    case Op::SYSENTERBW:
    case Op::SYSEXITDW:
    case Op::SYSCALLDW:
    case Op::SYSENTERDW:
    case Op::SYSEXITFW:
    case Op::SYSCALLFW:
    case Op::SYSENTERFW:
    case Op::SYSEXITPW:
    case Op::SYSCALLPW:
    case Op::SYSENTERPW:
      throw std::runtime_error("Unhandled Op");
      break;
  }
  throw std::runtime_error("Invalid Op");
}
}  // namespace kuso::x86