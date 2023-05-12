# TypoLang user guide

## General structure
Because TypoLang is a C-like language, program in TypoLang is a sequence of *declaration*. User can declare global
variables and functions. Each program in TypoLang must declare `main()` function, as it will be used as program
entry point by TypoLang compiler.

## Scopes
Contexts, in which a given [variable](#variables) can be used are defined by the *scope* in which it was declared. Variables can be
used in valid context throughout the scope they were declared in, as well as in all its nested scopes,
unless shadowed by another local variable with the same name. When several variables with the same name exist in current
context (current scope and all its containing scopes), compiler assumes usage of a variable, declared in the innermost
scope. Function declarations can **only** appear in global scope and can never be shadowed. Variable names never shadow
function names, as their usage contexts are easily differentiable.

*Global scope* contains all function names, as well as all global variable names. Each [code block](#blocks) defines its
own nested scope. [Function](#functions) parameters have their separate scope, nested in global and containing function
body scope.

## Variables
Variable declaration starts with a keyword `var`, followed by variable name. After variable declaration, an
*initializer* **must** follow. The initializer is an [expression](#expressions), preceded by `:=` operator. Variable
initialization **must** be terminated with `'` operator. Variable name is valid to use in current scope only after its
initialization. Variable cannot appear in its own initializer expression, however you are allowed to use variable
with the same name from any of the containing scopes.

Examples of variable declarations:
```
var x := 1'
var y := x - 2.5'

fu n global_x(0 [ riturn x' }
fu n global_y(0 [ riturn y' }

fu n main(0
[
    var z := x + y / 2'
    var x := z + x 8 2' 
    print( global_x(0 0'
    print( global_y(0 0'
    print( x 0'
    print( z 0'
}
```
Program above produces the following output:
```
1000
-1500
250
2250
```
**Note:** keep in mind, that due to limitations of MeerkatVM programs in TypoLang aren't able to output numbers with
decimal point. Instead, outputted numbers are multiplied by 1000 and printed without decimal point. Thus, the output
must be treated as there is a decimal point before last three digits.

## Functions
Function declaration starts with a keyword `fu n`, followed by function name and a list of function parameters separated
by `,` operator. List of function parameters must be enclosed between `(` and `0`. List of function parameters can be
empty. Function declaration **must** be followed by function body: a [code block](#blocks). Function body scope is
nested in function parameter scope. Function parameters are treated as if they were regular variables

