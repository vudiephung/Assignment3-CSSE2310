#include "utils.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

char* number_to_string(int num) {
    int ditgitsCount = count_number_digit(num);
    char* numStr = malloc(sizeof(char) * ditgitsCount);
    sprintf(numStr, "%d", num);
    return numStr;
}

int count_number_digit(int num) {
    int count = 0;
    do {
        count++;
        num /= 10;
    } while (num != 0);
    return count;
}

int find_max(int* array, int size) {
    int max = array[0];
    for (int i = 1; i < size; i++) {
        if (array[i] > max) {
            max  = array[ i ];
        }
    }
    return max;
}

bool is_digits_only(const char* str, int* value) {
    char tempChar;
    return (sscanf(str, "%d%c", value, &tempChar) == 1);
}

bool read_line(FILE* file, char* buffer, int* size) {
    int count = 0;
    buffer[0] = '\0';
    int next;

    while (true) {
        next = fgetc(file);
        if (next == EOF || next == '\n') {
            buffer[count] = '\0';
            break;
        } else {
            if (count + 2 > *size) {
                int biggerSize = *size * 1.5;
                void* newBuffer = realloc(buffer, biggerSize);
                if (newBuffer == 0) {
                    return false;
                }
                *size = biggerSize;
                buffer = newBuffer;                
            }
            buffer[count++] = (char)next;
        }
    }

    return next != EOF;
}