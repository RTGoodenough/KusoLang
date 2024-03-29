/**
 * @file linux.hpp
 * @author Rolland Goodenough (goodenoughr@gmail.com)
 * @date 2023-12-19
 *
 * @copyright Copyright 2023 Rolland Goodenough
 *
 * This file is part of kuso which is released under the MIT License
 * See file LICENSE for the full License
 */

#pragma once

namespace kuso::lnx {

/**
 * @brief Linux System Call values
 * 
 */
enum Syscall {
  READ = 0,
  WRITE = 1,
  OPEN = 2,
  CLOSE = 3,
  STAT = 4,
  FSTAT = 5,
  LSTAT = 6,
  POLL = 7,
  LSEEK = 8,
  MMAP = 9,
  MPROTECT = 10,
  MUNMAP = 11,
  BRK = 12,
  RT_SIGACTION = 13,
  RT_SIGPROCMASK = 14,
  RT_SIGRETURN = 15,
  IOCTL = 16,
  PREAD64 = 17,
  PWRITE64 = 18,
  READV = 19,
  WRITEV = 20,
  ACCESS = 21,
  PIPE = 22,
  SELECT = 23,
  SCHED_YIELD = 24,
  MREMAP = 25,
  MSYNC = 26,
  MINCORE = 27,
  MADVISE = 28,
  SHMGET = 29,
  SHMAT = 30,
  SHMCTL = 31,
  DUP = 32,
  DUP2 = 33,
  PAUSE = 34,
  NANOSLEEP = 35,
  GETITIMER = 36,
  ALARM = 37,
  SETITIMER = 38,
  GETPID = 39,
  SENDFILE = 40,
  SOCKET = 41,
  CONNECT = 42,
  ACCEPT = 43,
  SENDTO = 44,
  RECVFROM = 45,
  SENDMSG = 46,
  RECVMSG = 47,
  SHUTDOWN = 48,
  BIND = 49,
  LISTEN = 50,
  GETSOCKNAME = 51,
  GETPEERNAME = 52,
  SOCKETPAIR = 53,
  SETSOCKOPT = 54,
  GETSOCKOPT = 55,
  CLONE = 56,
  FORK = 57,
  VFORK = 58,
  EXECVE = 59,
  EXIT = 60,
};

}  // namespace kuso::lnx