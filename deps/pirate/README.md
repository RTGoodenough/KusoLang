# pirate

C++ header only command line arguments parsing library, I use for my personal projects

# How To Use

## Including

To use `pirate` simply include the header file.

```c++
#include <pirate.hpp>
```

## Registering Arguments

Arguments need to be registered with `pirate` before they can be parsed from the command line.

Arguments are registered with the methods:
```c++
pirate::Args::register_arg(const std::string&)
// or
pirate::Args::register_arg(const std::string&, pirate::ArgType)
```

```c++
// This registers an optional argument "example"
pirate::Args::register_arg("example");

// This registers an argument "another" that is required
pirate::Args::register_arg("another", pirate::ArgType::REQUIRED);
```

### ArgTypes

Arguments can be optional, or have an optional value; this is done through the `ArgType` enum.

```c++
ArgType::OPTIONAL // Argument is not required to run the program
ArgType::REQUIRED // Argument is required, exception is thrown if not provided
ArgType::VALUE_REQUIRED // Argument requires a value, exception is thrown if not provided
```
All arguments are set as `OPTIONAL` by default.

```c++
// "another" is not required; but if it is provided, it requires a value
pirate::Args::register_arg("another",ArgType::VALUE_REQUIRED);
```

A `ArgType` can be combined with `|`
```c++
// "example" is required and also requires a value
pirate::Args::register_arg("example", ArgType::REQUIRED | ArgType::VALUE_REQUIRED);
```

## Using Parsed Arguments

Arguments are accessed through the method
```c++
auto get(std::string_view) -> const std::string&;

// example:
const std::string& exampleArg = pirate::Args::get("example");
```

If the argument does not have a value, the empty string `""` will be returned.

Optional arguments should be checked before using `get` with
```c++
auto has(std::string_view) -> bool;

// example:
bool hasExample = pirate::Args::has("example");
```