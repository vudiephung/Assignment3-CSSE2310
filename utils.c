#include "utils.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

char* number_to_string(int number) {
    int ditgitsCount = count_number_digit(number);
    char* numberAsString = malloc(sizeof(char) * ditgitsCount);
    sprintf(numberAsString, "%d", number);
    return numberAsString;
}

int count_number_digit(int num) {
    int count = 0;
    do {
        count++;
        num /= 10;
    } while (num != 0);
    return count;
}

int find_max(int* array, int size, int* index) {
    int max = array[0];
    if (index != NULL) {
        *index = 0;
    }
    for (int i = 1; i < size; i++) {
        if (array[i] > max) {
            max = array[i];
            if (index != NULL) {
                *index = i;
            }
        }
    }
    return max;
}

void swap(int* firstNumber, int* secondNumber) { 
    int temp = *firstNumber; 
    *firstNumber = *secondNumber; 
    *secondNumber = temp; 
} 
  
// A function to implement bubble sort 
void bubble_sort(int* array, int size) { 
    int i, j; 
    for (i = 0; i < size - 1; i++) {
        for (j = 0; j < size - i - 1; j++) {
            if (array[j] > array[j + 1]) {
                swap(&array[j], &array[j + 1]); 
            }
        }
    }
} 

bool is_digits_only(const char* myString, int* value) {
    char temporaty;
    return (sscanf(myString, "%d%c", value, &temporaty) == 1);
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