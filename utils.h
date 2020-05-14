#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

char* number_to_string(int num);

int count_number_digit(int num);

int find_max(int* array, int size, int* index);

void swap(int* firstNumber, int* secondNumber);

void bubble_sort(int* array, int n);

bool is_digits_only(const char* str, int* value);

bool read_line(FILE* file, char* buffer, int* size);

#endif