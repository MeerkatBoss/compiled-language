#ifndef SIZE_ARRAY_H
#define SIZE_ARRAY_H

#include <stddef.h>

#define ARRAY_ELEMENT size_t
#include "dynamic_array.h"
#undef ARRAY_ELEMENT

#endif