# COMMANDMENTS

Follow it and all will be ok.

- Each commitment must make sense.

- Never build the project: do not run `ninja`, `cmake --build`, `make`, or any other build command.
- Never run `clang-format`.  
- Only check C++ and QML code via LSP (diagnostics). Do not verify by building.  
- Never clean project.  
- No ternary operators, only explicit "if" statements.  
- No "short" if statments, always brace-enclosed.  

- Do not commit changes, the one do it by himself.
- Do not run an aplication, if you need check only build it, the one run code by himself.

- If a plan is implemented, give a good middle-sized commit message in MD format.
- Commit message must be "pastable", do not use extra characters or guard them or give full pastable git command.

- Start all class data members with `_` in camelCase.
- Obviously, not use this pattern for private and protected class methods!
- All "user facing" properties of class (that have explicit getters and setters) must have exact name (no underscore
  or "m")

- No ternary operators, only explicit `if` statements.
- Use maximum verbose lambda syntax.
- Class declaration must preamble with 79-char three-line class preamble banners - commented quals sign line, class name
  enclosed with equals signs line end enclosing equals signs line (do not use that pattern for nested classes), place empty line after preable.
- Always use trailing return type syntax for functions and class methods only except for Q_INVOKABLE marked methods.

- Do not use comment style with three slashes like "///", use two slashes or classic C lang comments "/* ... */"
- Use curly brace (unicorn) initialization style.
- If a class method placed in header file and it body occupy only one string - place a double slash "//" symbol at first line and next line must be a function body (for appropriate formatting with existing formatting rules))
- Use `auto`, `const auto` (same with star or ref qualification) when appropriate. 
- Prefer `const auto foo{0}` style initialization instead of assignment operator.
- Prefer `size_t` type for counter in loops.
- Always use c++ style casts! Deprecate c-style casts.

- Prefer struct over class object declaration.
- Never use private, protected or virtual inheritance.
- Never use multiple inheritance, except inharitance only from multiple pure virtual bases (interfaces).

- Prefer `#pragma once` guard over ifdefs.

- Do not touch `NOTE` and `TODO` that the one leaves across codebase.
- Do not use ancient c++ patterns like private constructors or something, use latest c++ fetures, final classes, defalted and deleted speacial members and other.
- Place class methods and class members like this - public then protected then private, do not mess a data members and class mthods in same group, keep it under different acces modifiers groups.
- Special class methods must be defined first, then section for public, protected and private methods, then same for data members.
- When exceptions is used never throw nothing except simple `runtime_error`, i do not needed in exception types garbage.
- It is not necessary to return const references to class data memebers in class accesors - always return by value.
- Place declarations in header and definition in cpp files always!

- Use standart c++ library facilities, do use library specific types only if library strongly requires it!
- Prefer std::format string facilities over old strams API. Prefer stringstream only as performans goal. 
- Use std::numeric_limits instead of hardcoded values.
- Use uppercase enum values.
- Ignore bad_alloc like it never happens, this is world wide non recoverable failure.
- Never use `catch(...)`
