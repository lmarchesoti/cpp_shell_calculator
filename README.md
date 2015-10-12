# cpp_shell_calculator
Interactive Shell Calculator in C++ using Boost Libraries

## Features

Integer, float and double precisions by the -(i/f/d) command.

+, -, \* and / operations along with parentheses. add, sub, mul and div aliases can also be used.

[M#] command to insert result into memory position #. Will create a new one in sequence if the number is too large.

clear M# to clear # memory position

clear command

exit command

## Libraries

This calculator uses the c++ boost libraries. More specifically:

boost/xpressive to parse the command line through regex

boost/lexical\_cast for safer type casts ;)

## Compilation

gcc -I *path\_to\_boost* src/\*.cpp

## Description

**main.cpp** accepts a parameter telling the initial precision to use(defaults to float). It then instantiates the calculator of the Calculator type.

**Calculator.h** is the header file for the Calculator class.

**Calculator.cpp** is the main file for the calculator. It parses the aforementioned commands. Every time an expression is entered, it is converted to postfix via a simplified Shunting Yard algorithm(because it doesn't have all operations and functions) and put on a stack. It is then evaluated and set to the accumulator register(memory position 0).
