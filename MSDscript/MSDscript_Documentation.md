# MSDscript Documentation



## Overview

MSDscript is an interpreter application built with C++. This interpreter supports functionalities as listed below:

1. Parsing: parse string input to MSDscript language structure.
2. Addition: calculate sum of numbers.
3. Multiplication: calculate product of numbers.
4. Boolean: support true or false type of values.
5. Comparison: compare variables or structures, get true or false values.
6. Variable assignment (let...in...) strutucture: support "_let \<expression\> _in \<expression\>" structure.
7. Condition (if...then...else...) structure: support "_if \<condition\> _then \<result 1\> _else \<result 2\>" structure.
8. Function structure and calculate: support "_fun \<function name\> \<function expression\>" sturcture, and support its usage



## Getting Started

\* currently supports only <u>MacOS or Linux</u> operating systems.

Please follow these steps in order:

1. In the terminal or command line tools, navigate to the folder contains MSDscript files.
2. Use `$ make` to compile the `MSDscript` executable. Be sure inside the same folder, an executable names `msdscript` is generated in this step.
3. After compilation, use `$ ./msdscript` to start running the executable.



## User Guide

After running `MSDscript`, user can simply type expressions in the terminal and let MSDscript do the parse and calculation.

### General Guide

The `MSDscript` supports `Step` mode or regular mode. `Step` mode prevents segmentation faults for larger recursive calls. 

#### Run regular mode

1. Choose the mode you would like to run:

​			Type `$ ./msdscript --help` to view help informations.

​			Type `$ ./msdscript --interp` to interp an expression.

​			Type `$ ./msdscript --print` to print an expression in MSDscript rules.

​			Type `$ ./msdscript --pretty-print` to print an expression in MSDscript rules in a more human readable way.

2. After typing, press `Enter` to run the MSDscript.

3. Now you can enter what you would like to interp or print. Space and new lines are supported. 

4. If you press `Control + D`, the MSDscript starts working by reading what you have input.

#### Run the `Step` mode

User can just simply add `--step` keywords in terminal command.

1. Choose the mode you would like to run:

​			Type `$ ./msdscript --step --interp` to interp an expression.

​			\* notice that: `$ ./msdscript --step --print` and  `$ ./msdscript --step --pretty-print` is also allowed. 		

2. After typing, press `Enter` to run the MSDscript.

3. Now you can enter what you would like to interp or print. Space and new lines are supported. 

4. If you press `Control + D`, the MSDscript starts working by reading what you have input.

### Supported Expression

#### Numbers (`NumExpr`)

In MSDscript, signed integer numbers are supported, they are called `NumExpr`.

To use a `NumExpr`, type number without spaces, commas, decimals or any other symbols.

**Example**

Valid input: `3`, `-3200`, `100000`

Invalid input: `3.0`, `-3,200`,  `100 000`

#### Strings (`VarExpr`)

In MSDscript, strings (variables) are supported and named `VarExpr`.

To use a `VarExpr`, type string without numbers, spaces, commas or other symbols except `_`. Notice that `_` can be used between letters or at the end of the string, but cannot be the put at the beginning.

**Example**

Valid input: `StudentNum`, `student_score`, `msd_tests_`

Invalid input: `Student Num`, `student-score`, `msd_tests2`

#### Boolean (`BoolExpr`)

In MSDscript, booleans are supported and named `BoolExpr`.

To use a `BoolExpr`, type `_true` or `_false`. Note that `_` must be included, else it will be interpreted as a `VarExpr`.

**Example**

Valid input: `_true`, `_false`

Invalid input: `true`, `false`, `0`,  `1`

#### Addition (`AddExpr`)

In MSDscript, additions are supported and named `AddExpr`.

To use an `AddExpr`, type left-hand side expression, `+` and then follow the right-hand side expression. Spaces and parathesis are supported.

**Example**

Valid input: `a + b`, `a+b`, `100 + 200`, `(100 + 200)`, `a + (1+2)`

Invalid input: `(a + 2`, `b + 1)`, `+2`, `a+`

#### Multiplication (`MultExpr`)

In MSDscript, multiplication are supported and named `MultExpr`.

To use a `MultExpr`, type left-hand side expression, `*` and then follow the right-hand side expression. Spaces and parathesis are supported.

**Example**

Valid input: `a * b`, `a*b`, `100 * 200`, `(100 * 200)`, `a * (1*2)`

Invalid input: `(a * 2`, `b * 1)`, `*2`, `a*`

#### Comparison (`EqualExpr`)

In MSDscript, comparison are supported and named `EqualExpr`.

To use a `EqualExpr`, type left-hand side expression, `==` and then follow the right-hand side expression. Spaces and parathesis are supported.

**Example**

Valid input: `a == b`, `a==b`, `100 == 200`, `(100 == 200)`, `a == (1*2)`

Invalid input: `(a == 2`, `b == 1)`, `==2`, `a==`

#### Variable Assignment (`LetExpr`)

In MSDscript, variable assignment are supported and named `LetExpr`.

To use a `LetExpr`, follow this structure:

```
_let <variableName> = <value>
_in <variable be utilized>
```

**Example**

Valid input:

```
1.
_let a = 2
_in a + 4
// if the user is running interp, then the result will be 6.

2.
_let b = 3+7 _in 1
// if the user is running interp, then the result will be 1.
```

Invalid input:

```
1.
_let 5
_in 3+a
// this only works if in the upper level the user has defined a's value

2.
_let a = 4 _in b
// this only works if in the upper level the user has defined b's value
```

#### Condition (`IfExpr`)

In MSDscript, conditions are supported and named `IfExpr`.

To use an `IfExpr`, follow this structure:

```
_if <condition>
_then <expression> // execute this line if <condition> is true
_else <expression> // execute this line if <condition> is false
```

**Example**

Valid input:

```
1.
_if 2 == 1+1
_then a
_else b
// if the user is running interp, then the result will be a's value.

2.
_if _false _then 100 _else -100
// if the user is running interp, then the result will be -100.
```

Invalid input:

```
1.
_if 2 = 1+1
_then a
_else b
// condition part after if must be an EqualExpr or BoolExpr

2.
_if 1 _then 100 _else -100
// condition part does not support using 1 or 0 to represent true and false.
```

#### Function (`FunExpr`)

In MSDscript, conditions are supported and named `FunExpr`.

To use an `FunExpr`, follow this structure:

```
1. To define a function
_fun (<input>) <expression>
// notice that the parathesis around <input> are needed.

2. To execute a function
(_fun (<input>) <expression>)(<inputExpr>)
```

**Example**

Valid input:

```
1.
(_fun (x) x*2)(5)
// if the user is running interp, then the result will be 10.

2.
_fun (y) y+2
// this defines a function
```

Invalid input:

```
_fun (x) x*2 (5)
```

### Files

#### Functional classes files

- `val.cpp` and `val.hpp`: defines value types.
- `expr.cpp` and `expr.hpp`: defines expressions and there structures.
- `env.cpp` and `env.hpp`: defines MSDscript expression's environments (for refering variable's values).
- `cont.cpp` and `cont.hpp`: defines the step mode interpretation.
- `step.cpp` and `step.hpp`: defines the step mode interpretation.

#### Helper classes files

- `main.cpp`: runable class that allows msdscript to be run on terminal.
- `parse.cpp` and `parse.hpp`: allows msdscript to read from the terminal and parse human readble strings to MSDscript expressions.
- `pointer.hpp`: allows MSDscript to switch between using or not using shared pointers.

