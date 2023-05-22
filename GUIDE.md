# TypoLang user guide

## General structure
Because TypoLang is a C-like language, program in TypoLang is a sequence of
*declarations*. User can declare global variables and functions. Each program in
TypoLang must declare `main(0` function, as it will be used as program entry
point by TypoLang compiler. The return value of `main(0` is used as program
exit code.

## Declarations

### Global Variables

Global variable declaration starts with a keyword `var`, followed by variable
name.  After variable declaration, an *initializer* **must** follow. The
initializer is an [expression](#expressions), preceded by `:=` operator. Global
variable initialization **must** be terminated with `'` operator. Variable name
is valid to use in global scope only after its initialization. Variable cannot
appear in its own initializer expression.

### Functions
Function declaration starts with a keyword `fu n`, followed by function name and
a list of function parameters separated by `,` operator. List of function
parameters **must** be enclosed between `(` and `0`. List of function parameters
can be empty. Parameters are defined with `var` keyword followed by parameter
name. Function declaration **must** be followed by function body: a
[block statement](#statements). Function body [scope](#scopes) is nested in function
parameter scope. Function parameters are treated as if they were regular
variables.

## Keywords

The following keywords are reserved and cannot be used as function or variable
names:

- `fu n`    - function declaration
- `var`     - variable declaration
- `eef`     - conditional statement 
- `els`     - conditional statement, false branch
- `vile`    - pre-condition loop
- `riturn`  - function return statement
- `or`      - boolean 'or' operator
- `aand`    - boolean 'and' operator
- `not`     - boolean 'not' operator
- `d`       - differentiation operator

## Standard library functions

TypoLang standard library provides the following functions:

- `read(0`      - read number from `stdin`. The number must be written with
                    exactly three digits after decimal point **WITHOUT** the
                    decimal point itself
- `print(x 0`   - print number `x` to `stdout` in format described by `read(0`
- `sqrt(x 0`    - calculate the square root of `x` and return it

No function defined in TypoLang program can have the same name as any of the
functions in standard library.

## Constants

Constants of TypoLang are fixed-precision decimal rational numbers. The decimal
separator is decimal point '.'. Decimal point can be omitted along with the
following digits. If the decimal point is not omitted, it must be followed by at
least one decimal digit. The integer part of the number can never be omitted.
Numbers 0.000 and 8.000 must always be written with decimal point or else they
will be interpreted as closing parentheses or multiplication operator respectively.

## Identifiers

Any sequence of characters 'a-zA-Z0-9_' not starting with decimal digit is
interpreted as function or variable name. This means, that the closing
parentheses (`0`) and multiplication operator (`8`) must be separated from their
operands with a space, or else they will be interpreted as part of an identifier.

## Expressions

Expression is a sequence of operators, constants, and identifiers that
produce a value.

### TypoLang Operators

- **Arithmetic:**
  - Addition: `+`
  - Subtraction: `-`
  - Multiplication: `8`
  - Division: `/`
  - Unary negation: `-`
- **Grouping:** `(` and `0`
- **Logic:**
    - `or`          - boolean 'or'
    - `aand`        - boolean 'and'
    - `not`         - boolean 'not'
- **Comparison:**
    - `<`           - less than
    - `.`           - greater than
    - `<=`          - less or equal
    - `>=`          - greater or equal
    - `====`        - equal
    - `1=`          - not equal
- **Function call:**
  - *function-name*`(`*arg1*`,` *arg2*`,` ... `0` - call function *function-name*
                                                    with argumets *arg1*, *arg2*, ...
- **Differentiation:**
  - `d(` *expression* `0 / d` *var-name* - transform *expression* containing
                                            only arithmetic operators, grouping
                                            parentheses, variables, and constants
                                            to its derivative (variable *var-name*
                                            is differentiation variable)

Multiplication by zero might be optimized to constant 0.000 by middle-end,
meaning that multiplication of function call result by zero is undefined
behavior if the called function produces any side-effects.

Logic operators operate only with numbers 0.001 (true value) and 0.000 (false
value). Application of logic operators to other numbers is undefined behavior.

Comparison operators compare their operands and produce logic value as a result
(0.000 or 0.001).

Table 1 demonstrates precedence of various TypoLang operators.

| Operator                          | Precedence |
| --------------------------------- | ---------- |
| Grouping                          | 9          |
| Differentiation and function call | 8          |
| Unary `-`                         | 7          |
| `8` and `/`                       | 6          |
| `+` and `-`                       | 5          |
| Comparison                        | 4          |
| `not`                             | 3          |
| `aand`                            | 2          |
| `or`                              | 1          |

*Table 1. TypoLang operator precedence*

## Statements

Statement is a sequence of keywords and expressions which produce no value but
control the program execution.

### TypoLang Statements

- **Variable initialization** - define new variable in current scope with given
                                value
  - `var` *var-name* `:=` *expression* `'`
- **Variable assignment**     - change the value of previously defined variable
  - *var-name* `<_` *expression* `'`
- **Function call**           - call function and discard its return value
  - *function-call-expression* `'`
- **Function return**         - stop function execution and return given value
  - `riturn` *expression* `'`
- **Conditional statement**   - select statement to execute based on condition
  ```
    eef ( <logic-expression> 0
      <statement>
    els
      <statement>
  ```
  The `els` branch may be omitted.
- **Loop statement**          - execute statement as long as condition is true
  ```
  vile ( <logic-expression> 0
    <statement>
  ```
- **Block statement**         - execute sequence of statements
  ```
  {
    <statements>
  ]
  ```

## Scopes

Contexts, in which a given variable can be used are defined by the
*scope* in which it was declared. Variables can be used in expressions and
statements throughout the scope they were declared in, as well as in all its
nested scopes, unless shadowed by another local variable with the same name.
Whenever several variables with the same name exist in current context (current
scope and all its containing scopes), compiler assumes usage of a variable,
declared in the innermost scope.

*Global scope* contains all global variable names. Each block statement
defines its own nested scope. Function parameters have their
separate scope, nested in global and containing function body scope.

Function declarations cannot appear in scopes other than global. Their names
exist in their own context and can never be shadowed.


