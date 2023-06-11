#define dynamic_array(type) dynamic_array_##type

#define DEFINE_DYNAMIC_ARRAY(type)\
struct dynamic_array_##type \
{                           \
    type* data;             \
    size_t capacity;        \
    size_t size;            \
}

#define DEFINE_ARRAY_CTOR(type) \
void array_ctor(dynamic_array(type)* array)

#define DEFINE_ARRAY_DTOR(type) \
void array_dtor(dynamic_array(type)* array)

#define DEFINE_ARRAY_PUSH(type) \
void array_push(dynamic_array(type)* array, type element)

#define DEFINE_ARRAY_POP(type) \
void array_pop(dynamic_array(type)* array)

#define DEFINE_ARRAY_COPY(type) \
void array_copy(dynamic_array(type)* dest, const dynamic_array(type)* src)

#define DEFINE_ARRAY_GET(type) \
type* array_get_element(const dynamic_array(type)* array, size_t index)

#define DEFINE_ARRAY_FRONT(type) \
type* array_front(const dynamic_array(type)* array)

#define DEFINE_ARRAY_BACK(type) \
type* array_back(const dynamic_array(type)* array)

