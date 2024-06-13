#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include "header.h"

int main(int argc, char *argv[]) {
    char *expression = NULL;
    int binaryOption = 0;
    int decimalOption = 0;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'b':
                binaryOption = 1;
                break;

                case 'd':
                decimalOption = 1;
                break;

                default:
                printf("Unknown option\n");
                exit(1);
            }
            if (argv[i][2] != '\0') {
                printf("Unknown option\n");
                exit(1);
            }
        } else {
            if (expression != NULL) {
                printf("One single argument expected\n");
                exit(1);
            }
            expression = argv[i];
        }
    }

    IntExt result = ParseExpression(expression);
    PrintIntExt(result, binaryOption, decimalOption);
    FreeIntExt(result);
}

