#ifndef ARRAY_H
#define ARRAY_H

#define array_push(array, value)                                              \
    do {                                                                      \
        (array) = array_hold((array), 1, sizeof(*(array)));                   \
        (array)[array_length(array) - 1] = (value);                           \
    } while (0);

void* array_hold(void* array, int count, int item_size);
int array_length(void* array);
void array_free(void* array);

#endif

//array pushes new values to the array

//array hold allocates a new element when the array is null
//but as soon as the array is not empty it uses realloc to reallocates
//new positions as needed

//array length returns how many spaces are occupied in the array

//array free, frees the allocated space in memory