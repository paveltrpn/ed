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
  enclosed with equals signs line end enclosing equals signs line.
- Always use trailing return type syntax for functions and class methods only except for Q_INVOKABLE marked methods.

- Do not use comment style with three slashes like "///", use two slashes or classic C lang comments "/* ... */"
- Use curly brace (unicorn) initialization style.
- If a class method placed in header file and it body occupy only one string - place a double slash "//" symbol at first line and next line must be a function body (for appropriate formatting with existing formatting rules))
- Use `auto`, `const auto` (same with star or ref qualification) when appropriate. 
- Prefer `const auto foo{0}` style initialization instead of assignment operator.
- Prefer `size_t` type for counter in loops.
