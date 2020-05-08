#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

int count_number_digit(int num);
char* number_to_string(int num);
int find_max(int* array, int size, int* index);
int find_index_of_max(int* array, int size);
void swap(int *a, int *b) ;
void bubble_sort(int* array, int n);
int find_min(int* array, int size);
bool is_digits_only(const char* str, int* value);
bool read_line(FILE* file, char* buffer, int* size);

#endif