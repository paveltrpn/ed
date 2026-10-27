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
