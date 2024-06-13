# Big int expression calculator

This program is a personal project that calculates the result of mathematical expressions without size limit. Supported operations are addition (`+`), substraction (`-`), multiplication (`*`), division (`/`) and exponentiation (`^`). Regular mathmatical priorities are applied. `~` can be used before a number to indicate that the following value is negativer, as `-` is used for substraction. 

## How to use

`make` to compute program.

`./calculate "expression to calculate" [-d] [-b]`

Result will be outputted in decimal format.

-d option to print result's number of decimal digits.

-b option to print details about result representation.

Examples :

`./calculate "1-2+ ~3*(5^(5-2))"`

`./calculate "10^100000" -d -b`

## Limitations

- Computation time goes from seconds with numbers around 100 000 decimals, to minutes with number around 1 000 000 decimals.

- Some invalid mathematical expressions can still compute. See shunting yard algorithm for more details.

- Most of the execution time is spent computing decimal notation. Hexadecimal notation should therefore be implemented as a more efficient way of writting/reading/storing the numbers.

- Negative exponents and exponents over (2^32 - 1) will be rejected.

- No extended testing has been done yet.

## Implementation

### IntExt

`IntExt` is the type used to represent extended integers without size limitation. It contains the following fields.

- `uint32_t *digits` is an array containing the binary representation of the number. Least significant digit is stored first. 32 bits digits are used so that overflowing can be easily handled with 64 bits operations.

- `int length` is the length of the previous digits array.

- `int negative` indicates if the number is negative. 0 for positive or zero, 1 for negative.

### Operations

All basic operations are performed with naive algorithms, as one would do with pen and paper, except we are using digits between 0 and (2^32 - 1) instead of between 0 and 9. Thus there is a lot of room for optimization. Exponentiation is performed with binary exponentiation algorithm. Details can be found in code.

### Decimal printing

Decimal notation printing is performed with a clasical base conversion algorithm, from binary to decimal. The decimal format uses a chained list representation and numbers between 0 and (10^18 - 1) coded with 64 bits numbers.

### Parsing expression

Expression parsing is performed with shunting yard algorithm, to transform traditional infix notation to reverse polish notation (RPN) that can be more easily computed. Operations are performed on the RPN stack as soon as they are parsed from the shunting yard algorithm.