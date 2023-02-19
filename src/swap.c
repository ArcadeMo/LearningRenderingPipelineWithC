#include "swap.h"

void int_swap(int* a, int* b) { //need to recieve the value by reference aka just pointing to the location of the variable and not in directly since we can use it for multiple variables rather than just one time instance
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void float_swap(float* a, float* b) { //need to recieve the value by reference aka just pointing to the location of the variable and not in directly since we can use it for multiple variables rather than just one time instance
    float tmp = *a;
    *a = *b;
    *b = tmp;
}