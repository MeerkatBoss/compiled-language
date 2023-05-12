#include <stdlib.h>

#include "logger.h"

static const size_t DEFAULT_CAP = 16;

#define __DEF_HELPER(macro1, macro2) macro1(macro2)

__DEF_HELPER(DEFINE_ARRAY_CTOR, ARRAY_ELEMENT)
{
    *array = {
        .data = (ARRAY_ELEMENT*) calloc(DEFAULT_CAP, sizeof(ARRAY_ELEMENT)),
        .capacity = DEFAULT_CAP,
        .size = 0
    };
}

__DEF_HELPER(DEFINE_ARRAY_DTOR, ARRAY_ELEMENT)
{
    for (size_t i = 0; i < array->size; i++)
        delete_element(&array->data[i]);
    free(array->data);
    *array = {};
}

__DEF_HELPER(DEFINE_ARRAY_PUSH, ARRAY_ELEMENT)
{
    copy_element(&array->data[array->size], &element);
    array->size++;
    
    if (array->size == array->capacity)
    {
        array->capacity *= 2;
        array->data = (ARRAY_ELEMENT*) reallocarray(array->data, array->capacity, sizeof(ARRAY_ELEMENT));
    }
}

__DEF_HELPER(DEFINE_ARRAY_POP, ARRAY_ELEMENT)
{
    LOG_ASSERT(array->size > 0, return);

    delete_element(array_back(array));
    array->size--;

    if (array->size * 4 <= array->capacity && array->capacity > DEFAULT_CAP)
    {
        array->capacity /= 2;
        array->data = (ARRAY_ELEMENT*) reallocarray(array->data, array->capacity, sizeof(ARRAY_ELEMENT));
    }
}

__DEF_HELPER(DEFINE_ARRAY_COPY, ARRAY_ELEMENT)
{
    dest->size = src->size;
    dest->capacity = src->capacity;
    dest->data = (ARRAY_ELEMENT*) calloc(dest->capacity, sizeof(ARRAY_ELEMENT));

    for (size_t i = 0; i < dest->size; i++)
        copy_element(&dest->data[i], &src->data[i]);
}

__DEF_HELPER(DEFINE_ARRAY_GET, ARRAY_ELEMENT)
{
    LOG_ASSERT(index < array->size, return NULL);

    return &array->data[index];
}

__DEF_HELPER(DEFINE_ARRAY_FRONT, ARRAY_ELEMENT)
{
    LOG_ASSERT(array->size > 0, return NULL);
    return &array->data[0];
}

__DEF_HELPER(DEFINE_ARRAY_BACK, ARRAY_ELEMENT)
{
    LOG_ASSERT(array->size > 0, return NULL);
    return &array->data[array->size - 1];
}
#undef __DEF_HELPER