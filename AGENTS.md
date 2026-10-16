# Rules

- Never build the project: do not run `ninja`, `cmake --build`, `make`, or any other build command.
- Never run `clang-format`.  
- Only check C++ and QML code via LSP (diagnostics). Do not verify by building.  
- Never clean project.  

# Code  
- No ternary operators, only explicit "if" statements.  
- No "short" if statments, always brace-enclosed.  
