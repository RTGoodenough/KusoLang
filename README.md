# KusoLang

! This is a personal project and not meant to be used in any actual capacity !

A statically-typed, compiled programming language.

Skip to the `KusoLang` section below to read the language syntax/semantics.

---
# Morasu
The KusoLang Compiler

Compiles KusoLang source files to x86_64 assembly

## Building

The Morasu compiler can be compiled with `CMake`

```
cmake -B build .
cmake --build build
```
---
## Running
```
morasu -in=<path to source> [flags]
```

The resulting x86_64 assembly needs to be passed through another compiler like `nasm` to produce machine code.

Direct KusoLang to machine code is planned.

nasm example:
```
nasm -felf64 kuso.out && ld kuso.o
```

The resulting program can be executed with:
```
./a.out
```
---
### Flags

These are the current flags available for the Morasu compiler:

```
-out=<filepath> sets path of the output x86_64
  default: ./kuso.out
```
```
-log=<debug|info|warn|error> sets the level for command line output
  default: info
```
```
-s  silences all command line output except errors
```

---
# KusoLang

# Main

Each KusoLang program requires an entry point:

```
main {
  <statements>
};
```

Example:
```
main {
  a : int = 5;
  exit a;
};
```
---
# Variables

```
<name> : <type>;
```

Variables can be assigned in the same line as their declaration.

```
<name> : <type> = <expression>;
```

Example:
```
a : int;


a : int = 5;
```
---
## Assignment

```
<name> = <expression>;
```

Example:
```
a = 5;
```
---
# Expressions

```
<number>
<variable>
<expression> <+|-|*|\|==|!=|>|<|>=|<=> <expression>
```
---
# Loops

While loops loop until the expression evaluates to 0

```
while (<expression>) {
  <statements>
};
```

Example:
```
N : int = 5;

while (N > 0) {
  N = N - 1;
};

// alternatively

while (N) {
  N = N - 1;
}
```
---
# If/Else

`if` statements execute the statements in their body if the expression does not evaluate to 0.

the `if` statements corresponding `else` will be executed if the expression does not evaluate to 0.

```
if (<expression>) {
  <statements>
};


if (<expression>) {
  <statements>
} else {
  <statements>
};
```
---
# Functions

```
func <name>( <variable declarations> ) -> <return type> {
  <statements>
};
```

Example:
```
func negative(a : int) -> int {
  return -a;
};

func sum(a : int, b : int) -> int {
  return a + b;
};
```
---
## Calling Functions

Functions are called by their name along with a list of expressions matching the function's parameters.

```
<name>(<expressions>);
```

Example
```
main {
  a : int = 4;
  b : int = 3;

  c : int = sum(a, b);

  exit c;
};
```
---
# Inline Assembly

Inlined x86_64 can be added to account for any missing features.

```
asm {
  <assembly instructions>
};
```

Example:
```
asm {
  mov rax, 5
  mov rdi, 3
  add rax, rdi
};
```
---
## Example Fibonacci Sequence:

This program calculates the fibonacci sequence value for a given number.

```
func print(val : int) -> none {
  asm {
    mov    rax, rdi
    mov    rcx, 0xa
    push   rcx
    mov    rsi, rsp
    sub    rsp, 16

  .toascii_digit:
    xor    rdx, rdx
    div    rcx
    add    rdx, '0'
    dec    rsi
    mov    [rsi], dl
    test   rax,rax
    jnz  .toascii_digit
    mov    rax, 1
    mov    rdi, 1
    lea    rdx, [rsp+16 + 1]
    sub    rdx, rsi
    syscall
    add  rsp, 24
    ret
  };

  return;
};

func fibo(N : int) -> int {
  t1 : int = 0;
  t2 : int = 1;
  next : int = 0;

  while (N) {
    t1 = t2;
    t2 = next;
    next = t1 + t2;
    N = N - 1;
  };

  return next;
};

main {
  a : int = fibo(40);
  print(a);
  exit 0;
};
```
