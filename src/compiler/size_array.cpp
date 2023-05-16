#include "size_array.h"

#define ARRAY_ELEMENT size_t
static inline void copy_element(ARRAY_ELEMENT* dest, const ARRAY_ELEMENT* src) { *dest = *src; }
static inline void delete_element(ARRAY_ELEMENT* element) { *element = 0; }
#include "array/dynamic_array_impl.h"
#undef ARRAY_ELEMENT
