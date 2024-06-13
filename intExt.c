#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "header.h"

// Return IntExt with single digit equal to given value
IntExt InitiateIntExt(uint32_t value, int negative) {
    IntExt result;
    result.digits = malloc(sizeof(uint32_t) * 1);
    result.digits[0] = value;
    result.length = 1;
    result.negative = negative;

    return result;
}

// Return IntExt of given length with every digit equal to zero
IntExt InitiateIntExtZero(int length) {
    IntExt result;
    result.digits = malloc(sizeof(uint32_t) * length);
    result.length = length;
    result.negative = 0;

    for (int i = 0; i < length; i++) {
        result.digits[i] = 0;
    }

    return result;
}

// Return IntExt copy of value
IntExt DuplicateIntExt(IntExt value) {
    IntExt result;
    result.length = value.length;
    result.negative = value.negative;
    result.digits = malloc(sizeof(uint32_t) * value.length);

    for (int i = 0; i < value.length; i++) {
        result.digits[i] = value.digits[i];
    }

    return result;
}

// Free digit array of intExt
void FreeIntExt(IntExt intExt) {
    free(intExt.digits);
}

// Return intExt's digit of given rank
uint32_t GetDigit(IntExt intExt, int rank) {
    uint32_t result = rank < intExt.length? intExt.digits[rank] : 0;

    return result;
}

// Reduce intExt length to ignore useless head zeros
void RemoveHeadZeros(IntExt *intExt) {
    for (int i = intExt->length - 1; i > 0; i--) {
        if (intExt->digits[i] == 0) {
            intExt->length--;
        } else {
            return;
        }
    }
    if (intExt->length == 1 && intExt->digits[0] == 0) {
        intExt->negative = 0;
    } 
}

// Set intExt to zero
void Nullify(IntExt *intExt) {
    free(intExt->digits);
    intExt->digits = malloc(sizeof(uint32_t) * 1);
    intExt->digits[0] = 0;
    intExt->length = 1;
    intExt->negative = 0;
}
