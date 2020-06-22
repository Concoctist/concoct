# Contributing to Concoct

## Issue Tracker

https://github.com/ConcoctLang/concoct/issues

## Discord

https://discord.concoct.dev/

## Wiki

https://github.com/ConcoctLang/concoct/wiki

## Coding Convention

Please follow the existing styling as closely as possible for any code contributions. Here are some tips to follow:

* Use the C99 standard.

* `//` should be used for comments on a single line.

* Multi-line `/* ... */` comments are encouraged before functions to document them.

* Use tabs and not spaces to be consistent with the existing code.

* A single tab should represent 4 spaces.

* Custom types specified with `typedef` should have the first letter of their name capitalized.

* Please use Allman style for blocks. For example:
   ```c
   int func()
   {
       // ...
   }
   ```
   Rather than K&R style:
   ```c
   int func() {
       // ...
   }
   ```

* Use `const` when possible instead of `#define` for constants as it encourages stronger typing.

* Constant variable names should be fully capitalized. For example:
   ```c
   const double PI = 3.141592653589793238463;
   ```

* Variable and function names containing multiple words should be separated with an underscore `_` rather than using camelCase or PascalCase.

* `switch` cases with only a single statement should reside on a single line. For example:
   ```c
   switch(oc)
   {
       case OP_ADD: return "OP_ADD";
       // ...
   }
   ```
