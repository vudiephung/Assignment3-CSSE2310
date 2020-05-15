#include "utils.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// Convert given 'number' into string and return it
// e.g: number_to_string(123) returns "123"
char* number_to_string(int number) {
    int ditgitsCount = count_number_digit(number);
    char* numberAsString = malloc(sizeof(char) * ditgitsCount);
    sprintf(numberAsString, "%d", number);
    return numberAsString;
}

// From given 'number', count how many digits and return it
// e.g: count_number_digit(1234) returns 4
int count_number_digit(int number) {
    int count = 0;
    do {
        count++;
        number /= 10;
    } while (number != 0);
    return count;
}

// From given 'array' and its size, return the largest element from
// it. if 'index' not NULL, change its
// value into the index of largest element of the given array
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

// Swap the value of 2 pointers:
// 'firstNumber' and 'secondNumber'
// return void;
void swap(int* firstNumber, int* secondNumber) { 
    int temp = *firstNumber; 
    *firstNumber = *secondNumber; 
    *secondNumber = temp; 
} 
  
// Using Bubble Sort algorithm to sort the given 'array'
// and its 'size'
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

// return true iff given 'myString' contains only digit(s)
// dereference to 'value' and change its value with the gotten number
bool is_digits_only(const char* myString, int* value) {
    char temporaty;
    return (sscanf(myString, "%d%c", value, &temporaty) == 1);
}

// read a line from 'file' and save all read chars into 'buffer'
// realloc that buffer size if 'size' is not big enogugh
// return true if there is unexpected EOF
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