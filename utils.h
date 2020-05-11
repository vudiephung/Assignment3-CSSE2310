#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

// From given number, count how many digits and return it
// e.g: count_number_digit(1234) returns 4
int count_number_digit(int num);

// Convert given number into string and return it
// e.g: number_to_string(123) returns "123"
char* number_to_string(int num);

// From given array, return the largest element from it based on
// int size (length of that array). if int* index not NULL, change its
// value into the index of largest element of the given array
int find_max(int* array, int size, int* index);

// Swap the value of 2 addresses on memory
void swap(int* firstNumber, int* secondNumber);

// Using Bubble Sort algorithm to sort the given array of size n
void bubble_sort(int* array, int n);

// return the smallest element in the array
int find_min(int* array, int size);

// return true iff given char* contains only digit(s)
// dereference to int* value and change its value with the gotten number
bool is_digits_only(const char* str, int* value);

// read a line from FILE* file and save all read chars into buffer
// realloc that buffer size if *size is not big enogugh
// return true if there is unexpected EOF
bool read_line(FILE* file, char* buffer, int* size);

#endif