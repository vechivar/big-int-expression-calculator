#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "header.h"

void AddUnsigned(IntExt *base, IntExt term);
void SubUnsigned(IntExt *base, IntExt term);
int CompareAbsoluteValue(IntExt base, IntExt term);
int Compare32(uint32_t a, uint32_t b);
IntExt SingleDigitMultiply(IntExt intExt, uint32_t digit);
uint32_t ProcessDivision(IntExt *quotient, IntExt dividend);


// Calculate (base)^(power)
// Result is stored in base
void Exponent(IntExt *base, IntExt power) {
    // Perform binary exponentiation
    IntExt result = InitiateIntExt(1, 0);
    IntExt factor = DuplicateIntExt(*base);

    if (power.length != 1) {
        printf("Error : exponent out of range (size over 32 bits)\n");
        exit(1);
    }

    if (power.negative) {
        printf("Error : negative exponent\n");
        exit(1);
    }

    uint32_t power32 = power.digits[0];
    uint32_t oddPower = power32%2;

    while (power32 != 0) {
        if (power32%2) {
            Multiply(&result, factor);
        }

        Multiply(&factor, factor);
        power32 = power32>>1;
    }

    free(base->digits);
    base->digits = result.digits;
    base->length = result.length;
    if (base->negative && oddPower) {
        base->negative = 1;
    } else {
        base->negative = 0;
    }

    RemoveHeadZeros(base);

    free(factor.digits);
}

// Calculate (base)*(factor)
// Result is stored in base
void Multiply(IntExt *base, IntExt factor) {
// decompose intExt multiplication into a sum of digit * intExt multiplications
// works just the same as hand multiplications (don't forget the carry)
// ex : 
//          1   2   3   4
//  x               5   6
//  ------------------------
//          6*1 6*2 6*3 6*4 
//  +   5*1 5*2 5*3 5*4 0    
//  ------------------------     
//  ...      
// note : may reserve 1 digit more than needed, that won't be integrated in intExt length
    int resultSize = base->length + factor.length;
    IntExt result = InitiateIntExtZero(resultSize);

    // differentiate biggest and smallest number, to reduce amount of additions
    IntExt smallest, biggest;
    if (base->length > factor.length) {
        smallest = factor;
        biggest = *base;
    } else {
        smallest = *base;
        biggest = factor;
    }

    for (int i = 0; i < smallest.length; i++) {
        // multiply (biggest) by digit of rank i in (smallest)
        // complete with enough 0 at the end and add to result
        // repeat for each digit in smallest
        int termSize = i + biggest.length + 1;
        IntExt term = InitiateIntExtZero(termSize);

        uint64_t carry = 0, factor = (uint64_t) GetDigit(smallest, i);
        for (int j = 0; j < biggest.length; j++) {
            uint64_t multResult = factor * (uint64_t) GetDigit(biggest, j) + carry;
            term.digits[i + j] = (uint32_t) multResult;
            carry = multResult >> 32;
        }
        term.digits[i + biggest.length] = (uint32_t) carry;

        Add(&result, term);
        free(term.digits);
    }

    free(base->digits);

    base->digits = result.digits; 
    base->length = resultSize;
    base->negative = base->negative != factor.negative;
    RemoveHeadZeros(base);
}

// Calculate (base)+(term)
// Result is stored in base
void Add(IntExt *base, IntExt term) {
    if (base->negative == term.negative) {
        AddUnsigned(base, term);
    } else {
        int greaterThan = CompareAbsoluteValue(*base, term);

        switch (greaterThan) {
            case 1:
            SubUnsigned(base, term);
            break;

            case -1:
            IntExt result = DuplicateIntExt(term);
            SubUnsigned(&result, *base);
            FreeIntExt(*base);
            base->negative = result.negative;
            base->digits = result.digits;
            base->length = result.length;
            break;

            case 0:
            Nullify(base);
            break;
        }
    }
}

// Calculate (base)-(term)
// Result is stored in base
void Sub(IntExt *base, IntExt term) {
    term.negative = (-1 * term.negative) + 1;
    Add(base, term);
}

// Calculate (base)+(term), ignoring signs
// Result is stored in base
void AddUnsigned(IntExt *base, IntExt term) {
// decompose IntExt sum into simpler digit sums.
// works the same way as hand addition (don't forget the carry)
// note : may reserve 1 digit more than needed, that won't be integrated in intExt length
    int resultSize;
    if (base->length > term.length) {
        resultSize = base->length + 1;
    } else {
        resultSize = term.length + 1;
    }
    uint32_t *result = malloc(sizeof(uint32_t) * resultSize);

    uint64_t carry = 0;

    for (int i = 0; i < resultSize; i++) {
        uint64_t digit = (uint64_t) GetDigit(*base, i) + (uint64_t) GetDigit(term, i) + carry;
        result[i] = (uint32_t) digit;
        carry = digit >> 32;
    }

    free(base->digits);

    // reduce length if last digit is 0
    base->digits = result;
    base->length = resultSize;
    RemoveHeadZeros(base);
}

// Calculate (base)-(term), ignoring signs
// Result is stored in base
// Base should be greater in absolute value than term
void SubUnsigned(IntExt *base, IntExt term) {
    uint32_t carry = 0;
    int i = 0;
    while (i < term.length || carry) {
        uint32_t termDigit = GetDigit(term, i);
        uint32_t nextCarry = base->digits[i] < (termDigit + carry);
        base->digits[i] -= termDigit + carry;
        carry = nextCarry;
        i++;
    }

    RemoveHeadZeros(base);
}

// Returns  1 if |a| > |b|
// Returns -1 if |a| < |b|
// Returns  0 if |a| = |b|
int CompareAbsoluteValue(IntExt a, IntExt b) {
    int result = Compare32(a.length, b.length);
    if (result != 0) {
        return result;
    }

    for (int i = a.length - 1; i >= 0; i--) {
        result = Compare32(a.digits[i], b.digits[i]);
        if (result != 0) {
            return result;
        }
    }

    return 0;
}

// Returns  1 if a > b
// Returns -1 if a < b
// Returns  0 if a = b
int Compare32(uint32_t a, uint32_t b) {
    if (a > b) {
        return 1;
    }

    if (a < b) {
        return -1;
    }

    return 0;
}

// Calculate (base)/(term)
// Result is stored in base
void Divide(IntExt *base, IntExt dividend) {
    // decompose IntExt division into simpler divisions with single digit result
    // works the same as hand euclidian division

    if (CompareAbsoluteValue(*base, dividend) == -1) {
        Nullify(base);
        return;
    }

    int resultSize = base->length - dividend.length + 1;
    IntExt result = InitiateIntExtZero(resultSize);

    // initiate subquotient from most significant digits of base
    // subquotients lengths can be (dividend.length) or (dividend.length + 1)
    IntExt subQuotient = InitiateIntExtZero(dividend.length + 1);
    for (int i = 0; i < dividend.length; i++) {
        subQuotient.digits[i] = base->digits[base->length - dividend.length + i];
    }
    subQuotient.length--;   // only (dividend.length) digits used here

    int lastDigitProcessed = base->length - dividend.length - 1;    // last base digit processed

    for (int i = resultSize - 1; i >= 0; i--) {
        // find next digit and update subquotient
        result.digits[i] = ProcessDivision(&subQuotient, dividend);
        if (lastDigitProcessed >= 0) {
            // compute next subquotient
            // shift all digits and use digit of rank lastDigitProcessed as least significant digit
            for (int j = dividend.length; j >= 1; j--) {
                subQuotient.digits[j] = subQuotient.digits[j-1];
            }
            subQuotient.digits[0] = base->digits[lastDigitProcessed];
            lastDigitProcessed--;

            // make sure length is properly set
            subQuotient.length = dividend.length + 1;
            RemoveHeadZeros(&subQuotient);
        }
    }

    FreeIntExt(subQuotient);
    free(base->digits);
    base->digits = result.digits;
    base->length = result.length;
    base->negative = base->negative != dividend.negative;
    RemoveHeadZeros(base);
}

// returns the greatest int p such as (p * dividend) <= quotient
// quotient is updated with the rest of the division : quotient = quotient - (p * dividend)
uint32_t ProcessDivision(IntExt *quotient, IntExt dividend) {
    uint32_t result = 0;
    int bitRank = 31;

    uint32_t subResult;
    IntExt subMult;

    // test result's bits one by one
    while (bitRank >= 0) {;
        subResult = result + (1 << bitRank);
        subMult = SingleDigitMultiply(dividend, subResult);
        if (CompareAbsoluteValue(subMult, *quotient) <= 0) {
            result = subResult;
        }
        bitRank--;
        FreeIntExt(subMult);
    }

    subMult = SingleDigitMultiply(dividend, result);
    SubUnsigned(quotient, subMult);
    FreeIntExt(subMult);

    return result;
}

// Returns digit * intExt
IntExt SingleDigitMultiply(IntExt intExt, uint32_t digit) {
    IntExt result = InitiateIntExtZero(intExt.length + 1);
    uint64_t carry = 0;

    for (int i = 0; i <= intExt.length; i++) {
        uint64_t mult = (uint64_t) digit * (uint64_t) GetDigit(intExt, i) + carry; 
        result.digits[i] = (uint32_t) mult;
        carry = (uint32_t) (mult >> 32);
    }

    RemoveHeadZeros(&result);
    return result;
}