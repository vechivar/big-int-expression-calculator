#include "header.h"
#include <stdio.h>
#include <stdlib.h>


// Reverse Polish Notation stack
// Output of shunting yard algorithm
typedef struct IntExtList {
    IntExt value;
    struct IntExtList *next;
} IntExtList;

void PushToRpnStack(IntExt value);
IntExt PopFromRpnStack();

IntExtList *rpnStack;


// Operator stack of shunting yard algorithm
typedef struct CharList {
    char operator;
    struct CharList *next;
} CharList;

void PushToOperatorStack(char operator);
char PopFromOperatorStack();

CharList *operatorStack;


// Program input reading
int currentIndice;
char *input;


void ProceedToken();
void ProceedOperator(char operator);
IntExt ReadNumber();
void ApplyOperation(char operator);

int GetPrecedence(char operator);

void ParsingError(char *msg);

// Main function for parsing program input
IntExt ParseExpression(char *arg) {
    // Initiate global values
    rpnStack = NULL;
    operatorStack = NULL;
    currentIndice = 0;
    input = arg;

    // Read and proceed every token
    while (input[currentIndice] != '\0') {
        ProceedToken();
    }

    // Apply remaining operators
    while (operatorStack != NULL) {
        if (operatorStack->operator == '(') {
            ParsingError("unmatching parenthesis");
        }
        ApplyOperation(PopFromOperatorStack());
    }

    // RPN stack should only contain the result
    if (rpnStack == NULL || rpnStack->next !=  NULL) {
        ParsingError("invalid stack after parsing expression");
    }

    IntExt result = rpnStack->value;
    free(rpnStack);
    return result;
}

// Read and proceed token from input
void ProceedToken() {
    char current = input[currentIndice];

    if (current == '\0') {
        return;
    } else if (current == ' ') {
        currentIndice++;
    } else if (GetPrecedence(current) != -1) {
        ProceedOperator(current);
        currentIndice++;
    } else {
        PushToRpnStack(ReadNumber());
    }

    return;
}

// Proceed operator according to shunting yard algorithm
void ProceedOperator(char operator) {
    switch (operator) {
        case '(':
        PushToOperatorStack(operator);
        break;

        case ')':
        while (operatorStack->operator != '(') {
            ApplyOperation(PopFromOperatorStack());
        }
        PopFromOperatorStack();
        break;

        default:
        int currentPrecedence = GetPrecedence(operator);
        while (operatorStack != NULL
                && operatorStack->operator != '('
                && GetPrecedence(operatorStack->operator) >= currentPrecedence
                ) {
            ApplyOperation(PopFromOperatorStack());
        }
        PushToOperatorStack(operator);
        break;
    }
}

// Read a number from input and convert it to IntExt format.
IntExt ReadNumber() {
    int length = 0;
    int negative = 0;

    if (input[currentIndice] == '~') {
        negative = 1;
        currentIndice++;
    }

    char current = input[currentIndice];

    while (current >= '0' && current <= '9') {
        length++;
        current = input[currentIndice + length];
    }

    if (length == 0) {
        // ReadNumber is default in ProceedToken, so length = 0 means no valid character was found
        ParsingError("unknown character");
    }

    // Convert input into IntExt
    IntExt result = InitiateIntExt(0, 0);
    IntExt powerOfTen = InitiateIntExt(1, 0);
    IntExt ten = InitiateIntExt(10, 0);

    for (int i = length - 1; i >= 0; i--) {
        current = input[currentIndice + i];
        if (current != '0') {
            IntExt digit = InitiateIntExt((uint32_t) (current - '0'), 0);
            Multiply(&digit, powerOfTen);
            Add(&result, digit);
            FreeIntExt(digit);
        }
        Multiply(&powerOfTen, ten);
    }

    FreeIntExt(powerOfTen);
    FreeIntExt(ten);

    result.negative = negative;

    currentIndice += length;

    return result;
}

// Reduce the two values on top of RPN stack by applying given operator
void ApplyOperation(char operator) {
    IntExt operand = PopFromRpnStack();

    void (*func)(IntExt*, IntExt);

    if (rpnStack == NULL) {
        ParsingError("not enough operands in stack");
    }

    switch(operator) {
        case '+':
        func = Add;
        break;

        case '-':
        func = Sub;
        break;

        case '*':
        func = Multiply;
        break;

        case '/':
        func = Divide;
        break;

        case '^':
        func = Exponent;
        break;
    }

    func(&rpnStack->value, operand);
    FreeIntExt(operand);
}

// Return operator precedence for shunting yard algorithm
int GetPrecedence(char operator) {
    switch (operator) {
        case '+':
        return 2;

        case '-':
        return 2;

        case '*':
        return 3;

        case '/':
        return 3;

        case '^':
        return 4;

        case '(':
        return 0;

        case ')':
        return 0;

        default:
        return -1;
    }
}

// Print message and exit program
void ParsingError(char *msg) {
    printf("Parsing error : %s\n", msg);
    exit(0);
}

// Push value on top of RPN stack
void PushToRpnStack(IntExt value) {
    IntExtList *new = malloc(sizeof(IntExtList));

    new->next = rpnStack;
    rpnStack = new;
    rpnStack->value = value;
}

// Return value on top of RPN stack and remove it from the stack
IntExt PopFromRpnStack() {
    if (rpnStack == NULL) {
        ParsingError("trying to pop from empty stack");
    }

    IntExt result = rpnStack->value;
    IntExtList *newStack = rpnStack->next;
    free(rpnStack);
    rpnStack = newStack;

    return result;
}

// Push oeprator on top of operator stack
void PushToOperatorStack(char operator) {
    CharList *element = malloc(sizeof(CharList));

    element->operator = operator;
    element->next = operatorStack;

    operatorStack = element;
}

// Return value on top of oeprator stack and remove it from the stack
char PopFromOperatorStack() {
    if (operatorStack == NULL) {
        ParsingError("empty operator stack");
    }

    char result = operatorStack->operator;

    CharList *stackTop = operatorStack;
    operatorStack = stackTop->next;
    free(stackTop);

    return result;
}
