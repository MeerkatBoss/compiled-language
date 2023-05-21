# TODO: complete guide

# TypoLang user guide

## General structure
Because TypoLang is a C-like language, program in TypoLang is a sequence of
*declaration*. User can declare global variables and functions. Each program in
TypoLang must declare `main()` function, as it will be used as program entry
point by TypoLang compiler.

## Reserved character sequences

### Keywords

The following keywords are reserved and cannot be used as function or variable
names:

- `fu n`    - function declaration
- `var`     - variable declaration
- `eef`     - conditional statement 
- `els`     - conditional statement, false branch
- `vile`    - pre-condition loop
- `riturn`  - function return statement

### Operators

This is the complete list of TypoLang operators, which are used to form the
expressions and statements of TypoLang:

- `[` and `}`   - block brackets
- `(` and `0`   - grouping parentheses
- `:=`          - initializer statement
- `<_`          - assignment statement
- `'`           - statement terminator
- `+`           - addition expression
- `-`           - subtraction expression
- `8`           - multiplication expression
- `/`           - division expression
- `or`          - boolean 'or'
- `aand`        - boolean 'and'
- `not`         - boolean 'not'
- `d`           - differentiation
- `,`           - function argument separator
- `<`           - 'less than` comparison expression
- `.`           - 'greater than` comparison expression
- `<=`          - 'less-equal` comparison expression
- `>=`          - 'greater-equal` comparison expression
- `====`        - 'equal` comparison expression
- `1=`          - 'not-equal` comparison expression

### Standard library functions

TypoLang standard library provides the following functions:

- `read(0`      - read number from `stdin`. The number must be written with
                    exactly three digits after decimal point **WITHOUT** the
                    decimal point itself
- `print(x 0`   - print number `x` to `stdout` in format described by `read(0`
- `sqrt(x 0`    - calculate the square root of `x` and return it

## Constants

Constants of TypoLang are fixed-precision decimal rational numbers. The decimal
separator is decimal point '.'. Decimal point can be omitted along with the
following digits. If the decimal point is not omitted, it must be followed by at
least one decimal digit. The integer part of the number cannot be omitted.
Numbers 0 and 8 must always be written with decimal point or else they will be
interpreted as closing parentheses or multiplication operator respectively.

## Identifiers

Any sequence of characters 'a-zA-Z0-9_' not starting with number is interpreted
as function or variable name. This means, that the closing parentheses `0` and
multiplication operand `8` must be separated from their operands with a space,
or else they will be interpreted as part of an identifier.

## Expressions

Expression is a sequence of keywords, operators, constants and identifiers that
produce a value.

## Operators

- **Arithmetic:**
  - Binary infix operators:
        - `+`           - addition
        - `-`           - subtraction
        - `8`           - multiplication
        - `/`           - division
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


## Scopes

Contexts, in which a given [variable](#variables) can be used are defined by the
*scope* in which it was declared. Variables can be used in expressions and
statements throughout the scope they were declared in, as well as in all its
nested scopes, unless shadowed by another local variable with the same name.
Whenever several variables with the same name exist in current context (current
scope and all its containing scopes), compiler assumes usage of a variable,
declared in the innermost scope.

*Global scope* contains all global variable names.  Each [code block](#blocks)
defines its own nested scope. [Function](#functions) parameters have their
separate scope, nested in global and containing function body scope.

Function declarations cannot appear in scopes other than global. Their names
exist in their own context and can never be shadowed.

## Variables

Variable declaration starts with a keyword `var`, followed by variable name.
After variable declaration, an *initializer* **must** follow. The initializer is
an [expression](#expressions), preceded by `:=` operator. Variable
initialization **must** be terminated with `'` operator. Variable name is valid
to use in current scope only after its initialization. Variable cannot appear in
its own initializer expression, however you are allowed to use variable with the
same name from any of the containing scopes.

## Functions
Function declaration starts with a keyword `fu n`, followed by function name and
a list of function parameters separated by `,` operator. List of function
parameters **must** be enclosed between `(` and `0`. List of function parameters
can be empty. Function declaration **must** be followed by function body: a
[code block](#blocks). Function body scope is nested in function parameter
scope. Function parameters are treated as if they were regular variables

