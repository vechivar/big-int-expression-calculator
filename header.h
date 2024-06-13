#include <stdint.h>

// extended int format, composed of multiple 32 bits components
// we use 32 bits digits so that we can simply handle airthmetic overflows by using 64 bits numbers
typedef struct IntExt {
    uint32_t *digits;   // array of 32 bits components (later called digits). first one is least significant
    int length;         // number of digits
    int negative;       // 0 if number is positive or nulle, 1 if negative
} IntExt;


IntExt InitiateIntExt(uint32_t value, int negative);
IntExt InitiateIntExtZero(int length);
IntExt DuplicateIntExt(IntExt intExt);
void FreeIntExt(IntExt IntExt);
uint32_t GetDigit(IntExt intExt, int rank);
void RemoveHeadZeros(IntExt *intExt);
void Nullify(IntExt *intExt);

void PrintIntExt(IntExt intExt, int binaryDetails, int decimalDetails);

void Add(IntExt *base, IntExt term);
void Sub(IntExt *base, IntExt term);
void Multiply(IntExt *base, IntExt factor);
void Divide(IntExt *base, IntExt dividend);
void Exponent(IntExt *base, IntExt power);

IntExt ParseExpression(char *argv);
