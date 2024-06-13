#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "header.h"

const uint64_t STRING_BASE = 1000000000000000000;   // maximum power of ten usable with uint64_t format
const int STRING_BASE_LENGTH = 18;

// Chained list to store decimal values while converting IntExt to decimal
// Head is least significant
typedef struct DecimalString {
    uint64_t value;                 // will be between 0 and STRING_BASE - 1
    struct DecimalString *next;
} DecimalString;


DecimalString *CreateDecimalString(uint64_t value);
void FreeDecimalString(DecimalString *string);

void PrintDecimal(IntExt intExt, int decimalDetails);
DecimalString *ComputeDecimalString(IntExt intExt);
int PrintDecimalString(DecimalString *string);

void Add_DecimalString(DecimalString *base, DecimalString *term, uint64_t carry);
void MultBy2_DecimalString(DecimalString *DecimalString, uint64_t carry);


// Print intExt decimal notation
// binaryDetails = true : also prints number of intExt digits and their values
// decimalDetails = true : also prints decimal length
void PrintIntExt(IntExt intExt, int binaryDetails, int decimalDetails) {
    if (binaryDetails) {
        printf("--Binary--\nLength : %d\n", intExt.length);
        if (intExt.negative) {
            printf("Negative\n");
        } else {
            printf("Positive\n");
        }
        printf("Digits :\n");
        for (int i = 0; i < intExt.length; i++) {
            printf("%lu  ", (unsigned long) intExt.digits[i]);
        }
        printf("\n");
    }

    PrintDecimal(intExt, decimalDetails);
}

// Return DecimalString with given value
DecimalString *CreateDecimalString(uint64_t value) {
    DecimalString *result = malloc(sizeof(DecimalString));
    result->value = value;
    result->next = NULL;
    return result;
}

// Recursively free all elements in string
void FreeDecimalString(DecimalString *string) {
    DecimalString *next = string->next;

    free(string);
    if (next != NULL) {
        FreeDecimalString(next);
    }
}

// Print decimal notation of intExt
// DecimalDetails = true : also prints decimal length
void PrintDecimal(IntExt intExt, int decimalDetails) {
    DecimalString *decimalDecimalString = ComputeDecimalString(intExt);

    if (decimalDetails) {
        printf("--Decimal--\n");
    }

    if (intExt.negative) {
        printf("-");
    }
    int decimalLength = PrintDecimalString(decimalDecimalString);
    printf("\n");

    if (decimalDetails) {
        printf("Length\n%d\n", decimalLength);
    }

    FreeDecimalString(decimalDecimalString);
}


// compute and return DecimalString from intExt by performing bit to bit decomposition.
DecimalString *ComputeDecimalString(IntExt intExt) {
    DecimalString *result = CreateDecimalString(0);   // decimal representation of intExt
    DecimalString *powers = CreateDecimalString(1);   // stores successive powers of 2

    for (int i = 0; i < intExt.length; i++) {
        uint32_t digit = intExt.digits[i];
        for (int j = 0; j < 32; j++) {
            if (digit % 2) {
                Add_DecimalString(result, powers, 0);
            }
            digit = digit >> 1;
            MultBy2_DecimalString(powers, 0);
        }
    }

    FreeDecimalString(powers);

    return result;
}

// Recursively print all values in DecimalString.
// Returns number of characters printed
int PrintDecimalString(DecimalString *string) {
    int result;

    if (string->next != NULL) {
        result = PrintDecimalString(string->next) + STRING_BASE_LENGTH;

        printf("%018llu", (unsigned long long) string->value);
    } else {
        result = 0;
        uint64_t digit = string->value;
        while (digit > 0) {
            result ++;
            digit = digit / 10;
        }

        printf("%llu", (unsigned long long) string->value);
    }

    return result;
}

// Recursively multiply each element in DecimalString by 2, pass carry to next element if needed
void MultBy2_DecimalString(DecimalString *string, uint64_t carry) {
    uint64_t value = string->value * 2 + carry;
    string->value = value % STRING_BASE;
    if (string->next != NULL) {
        MultBy2_DecimalString(string->next, value / STRING_BASE);
    } else if (value >= STRING_BASE) {
        // add last element with carry as value
        DecimalString *next = malloc(sizeof(DecimalString));
        next->value = value / STRING_BASE;
        next->next = NULL;
        string->next = next;
    }
}

// Recursively add DecimalString, pass carry to next element if needed
// Result is stored in base DecimalString
void Add_DecimalString(DecimalString *base, DecimalString *term, uint64_t carry) {
    uint64_t value = base->value + term->value + carry;
    base->value = value % STRING_BASE;

    if (base->next == NULL && term->next == NULL) {
        // add last element with carry as value
        if (value >= STRING_BASE) {
            base->next = CreateDecimalString(value / STRING_BASE);
        }
        return;
    }
    
    // extend shorter number with 0 if needed
    if (base->next == NULL) {
        base->next = CreateDecimalString(0);
    }
    
    if (term->next == NULL) {
        term->next = CreateDecimalString(0);
    }

    Add_DecimalString(base->next, term->next, value / STRING_BASE);
}
