#pragma once

#include <stdexcept>
#include <string>

namespace kuso::x86 {
enum Size { BYTE = 1, WORD = 2, DWORD = 4, QWORD = 8 };
enum class Register {
  RAX,
  RBX,
  RCX,
  RDX,
  RSI,
  RDI,
  RSP,
  RBP,
  R8,
  R9,
  R10,
  R11,
  R12,
  R13,
  R14,
  R15,
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
enum class Op {
  MOV,
  MOVL,
  ADD,
  SUB,
  MUL,
  IMUL,
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
  MOVZX,
  INT,
  SETGE,
  SETE,
  SETNE,
  SETG,
  SETL,
  SETLE,
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
  SYSENTERPW
};

struct Literal {
  int64_t   value{};
  x86::Size size{x86::Size::DWORD};

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