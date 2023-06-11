/**
 * @file asserts.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief Various assertion macros
 *
 * @version 0.1
 * @date 2023-04-06
 *
 * @TODO: Documentation
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __MEERKAT_ASSERTS_H
#define __MEERKAT_ASSERTS_H

#include <stddef.h>
#include <errno.h>

struct assert_failed_data
{
    bool had_error;

    size_t line_number;
    const char* filename;
    const char* function;
    const char* expression;
    const char* expectation;
    const char* message;
};

#define _STR(x) __STR(x)
#define __STR(x) #x

#define _CAT(x, y) __CAT(x, y)
#define __CAT(x, y) x##y

#define __GUARD_ITERATOR _CAT(__guard_iterator, __LINE__)
#define __SAFE_BLOCK_GUARD \
    for (size_t __GUARD_ITERATOR = 0; __GUARD_ITERATOR < 1; ++__GUARD_ITERATOR)

#define SAFE_BLOCK_START do {               \
    assert_failed_data assertion_info = {}; \
    __SAFE_BLOCK_GUARD

#define SAFE_BLOCK_HANDLE_ERRORS            \
    if (!assertion_info.had_error) break;   \
    __SAFE_BLOCK_GUARD

#define SAFE_BLOCK_END } while (0);

#ifndef NO_VERBOSE_ASSERTS
#define __FILL_ASSERT_DATA(action, condition, msg, data) do {       \
    assertion_info.line_number = __LINE__;                          \
    assertion_info.filename = __FILE__;                             \
    assertion_info.function = __PRETTY_FUNCTION__;                  \
    assertion_info.expression = #action;                            \
    assertion_info.expectation = #condition;                        \
    assertion_info.message = msg;                                   \
} while (0)
#else
#define __FILL_ASSERT_DATA(action, condition, msg, data) do {       \
    assertion_info.message = msg;                                   \
} while (0)
#endif

#define ASSERT_MESSAGE_CALLBACK(action, condition, message, callback) { \
    decltype(action) action_result = action;                            \
    if (!(condition))                                                   \
    {                                                                   \
        assertion_info.had_error = true;                                \
        callback;                                                       \
        __FILL_ASSERT_DATA(action, condition, message, data);           \
        break;                                                          \
    }                                                                   \
}

#define ASSERT_MESSAGE(action, condition, message) \
    ASSERT_MESSAGE_CALLBACK(action, condition, message, {})

#define ASSERT_CALLBACK(action, condition, callback) \
    ASSERT_MESSAGE_CALLBACK(action, condition, NULL, callback)

#define ASSERT_SIMPLE(action, condition) \
    ASSERT_MESSAGE_CALLBACK(action, condition, NULL, {})

#define ASSERT_ZERO_MESSAGE_CALLBACK(action, message, callback) \
    ASSERT_MESSAGE_CALLBACK(action, action_result == 0, message, callback)

#define ASSERT_ZERO_MESSAGE(action, message) \
    ASSERT_ZERO_MESSAGE_CALLBACK(action, message, {})

#define ASSERT_ZERO_CALLBACK(action, callback) \
    ASSERT_ZERO_MESSAGE_CALLBACK(action, NULL, callback)

#define ASSERT_ZERO(action) \
    ASSERT_ZERO_MESSAGE_CALLBACK(action, NULL, {})

#define ASSERT_NON_ZERO_MESSAGE_CALLBACK(action, message, callback) \
    ASSERT_MESSAGE_CALLBACK(action, action_result != 0, message, callback)

#define ASSERT_NON_ZERO_MESSAGE(action, message) \
    ASSERT_NON_ZERO_MESSAGE_CALLBACK(action, message, {})

#define ASSERT_NON_ZERO_CALLBACK(action, callback) \
    ASSERT_NON_ZERO_MESSAGE_CALLBACK(action, NULL, callback)

#define ASSERT_NON_ZERO(action) \
    ASSERT_NON_ZERO_MESSAGE_CALLBACK(action, NULL, {})

#define ASSERT_POSITIVE_MESSAGE_CALLBACK(action, message, callback) \
    ASSERT_MESSAGE_CALLBACK(action, action_result > 0, message, callback)

#define ASSERT_POSITIVE_MESSAGE(action, message) \
    ASSERT_POSITIVE_MESSAGE_CALLBACK(action, message, {})

#define ASSERT_POSITIVE_CALLBACK(action, callback) \
    ASSERT_POSITIVE_MESSAGE_CALLBACK(action, NULL, callback)

#define ASSERT_POSITIVE(action) \
    ASSERT_POSITIVE_MESSAGE_CALLBACK(action, NULL, {})

#define ASSERT_NEGATIVE_MESSAGE_CALLBACK(action, message, callback) \
    ASSERT_MESSAGE_CALLBACK(action, action_result < 0, message, callback)

#define ASSERT_NEGATIVE_MESSAGE(action, message) \
    ASSERT_NEGATIVE_MESSAGE_CALLBACK(action, message, {})

#define ASSERT_NEGATIVE_CALLBACK(action, callback) \
    ASSERT_NEGATIVE_MESSAGE_CALLBACK(action, NULL, callback)

#define ASSERT_NEGATIVE(action) \
    ASSERT_NEGATIVE_MESSAGE_CALLBACK(action, NULL, {})

#define ASSERT_NON_POSITIVE_MESSAGE_CALLBACK(action, message, callback) \
    ASSERT_MESSAGE_CALLBACK(action, action_result <= 0, message, callback)

#define ASSERT_NON_POSITIVE_MESSAGE(action, message) \
    ASSERT_NON_POSITIVE_MESSAGE_CALLBACK(action, message, {})

#define ASSERT_NON_POSITIVE_CALLBACK(action, callback) \
    ASSERT_NON_POSITIVE_MESSAGE_CALLBACK(action, NULL, callback)

#define ASSERT_NON_POSITIVE(action) \
    ASSERT_NON_POSITIVE_MESSAGE_CALLBACK(action, NULL, {})

#define ASSERT_NON_NEGATIVE_MESSAGE_CALLBACK(action, message, callback) \
    ASSERT_MESSAGE_CALLBACK(action, action_result >= 0, message, callback)

#define ASSERT_NON_NEGATIVE_MESSAGE(action, message) \
    ASSERT_NON_NEGATIVE_MESSAGE_CALLBACK(action, message, {})

#define ASSERT_NON_NEGATIVE_CALLBACK(action, callback) \
    ASSERT_NON_NEGATIVE_MESSAGE_CALLBACK(action, NULL, callback)

#define ASSERT_NON_NEGATIVE(action) \
    ASSERT_NON_NEGATIVE_MESSAGE_CALLBACK(action, NULL, {})

#define ASSERT_TRUE_MESSAGE_CALLBACK(action, message, callback) \
    ASSERT_MESSAGE_CALLBACK(action, action_result, message, callback)

#define ASSERT_TRUE_MESSAGE(action, message) \
    ASSERT_TRUE_MESSAGE_CALLBACK(action, message, {})

#define ASSERT_TRUE_CALLBACK(action, callback) \
    ASSERT_TRUE_MESSAGE_CALLBACK(action, NULL, callback)

#define ASSERT_TRUE(action) \
    ASSERT_TRUE_MESSAGE_CALLBACK(action, NULL, {})

#define ASSERT_FALSE_MESSAGE_CALLBACK(action, message, callback) \
    ASSERT_MESSAGE_CALLBACK(action, !action_result, message, callback)

#define ASSERT_FALSE_MESSAGE(action, message) \
    ASSERT_FALSE_MESSAGE_CALLBACK(action, message, {})

#define ASSERT_FALSE_CALLBACK(action, callback) \
    ASSERT_FALSE_MESSAGE_CALLBACK(action, NULL, callback)

#define ASSERT_FALSE(action) \
    ASSERT_FALSE_MESSAGE_CALLBACK(action, NULL, {})

#define ASSERT_EQUAL_MESSAGE_CALLBACK(action, value, message, callback) \
    ASSERT_MESSAGE_CALLBACK(\
            action, action_result == (value), message, callback)

#define ASSERT_EQUAL_MESSAGE(action, value, message) \
    ASSERT_EQUAL_MESSAGE_CALLBACK(action, value, message, {})

#define ASSERT_EQUAL_CALLBACK(action, value, callback) \
    ASSERT_EQUAL_MESSAGE_CALLBACK(action, value, NULL, callback)

#define ASSERT_EQUAL(action, value) \
    ASSERT_EQUAL_MESSAGE_CALLBACK(action, value, NULL, {})

#define ASSERT_NOT_EQUAL_MESSAGE_CALLBACK(action, value, message, callback) \
    ASSERT_MESSAGE_CALLBACK(\
            action, action_result != (value), message, callback)

#define ASSERT_NOT_EQUAL_MESSAGE(action, value, message) \
    ASSERT_NOT_EQUAL_MESSAGE_CALLBACK(action, value, message, {})

#define ASSERT_NOT_EQUAL_CALLBACK(action, value, callback) \
    ASSERT_NOT_EQUAL_MESSAGE_CALLBACK(action, value, NULL, callback)

#define ASSERT_NOT_EQUAL(action, value) \
    ASSERT_NOT_EQUAL_MESSAGE_CALLBACK(action, value, NULL, {})

#define ASSERT_GREATER_MESSAGE_CALLBACK(action, value, message, callback) \
    ASSERT_MESSAGE_CALLBACK(\
            action, action_result > (value), message, callback)

#define ASSERT_GREATER_MESSAGE(action, value, message) \
    ASSERT_GREATER_MESSAGE_CALLBACK(action, value, message, {})

#define ASSERT_GREATER_CALLBACK(action, value, callback) \
    ASSERT_GREATER_MESSAGE_CALLBACK(action, value, NULL, callback)

#define ASSERT_GREATER(action, value) \
    ASSERT_GREATER_MESSAGE_CALLBACK(action, value, NULL, {})

#define ASSERT_LESS_MESSAGE_CALLBACK(action, value, message, callback) \
    ASSERT_MESSAGE_CALLBACK(\
            action, action_result < (value), message, callback)

#define ASSERT_LESS_MESSAGE(action, value, message) \
    ASSERT_LESS_MESSAGE_CALLBACK(action, value, message, {})

#define ASSERT_LESS_CALLBACK(action, value, callback) \
    ASSERT_LESS_MESSAGE_CALLBACK(action, value, NULL, callback)

#define ASSERT_LESS(action, value) \
    ASSERT_LESS_MESSAGE_CALLBACK(action, value, NULL, {})

#define ASSERT_LESS_EQUAL_MESSAGE_CALLBACK(action, value, message, callback) \
    ASSERT_MESSAGE_CALLBACK(\
            action, action_result <= (value), message, callback)

#define ASSERT_LESS_EQUAL_MESSAGE(action, value, message) \
    ASSERT_LESS_EQUAL_MESSAGE_CALLBACK(action, value, message, {})

#define ASSERT_LESS_EQUAL_CALLBACK(action, value, callback) \
    ASSERT_LESS_EQUAL_MESSAGE_CALLBACK(action, value, NULL, callback)

#define ASSERT_LESS_EQUAL(action, value) \
    ASSERT_LESS_EQUAL_MESSAGE_CALLBACK(action, value, NULL, {})

#define ASSERT_GREATER_EQUAL_MESSAGE_CALLBACK(action, value, message, callback) \
    ASSERT_MESSAGE_CALLBACK(\
            action, action_result >= (value), message, callback)

#define ASSERT_GREATER_EQUAL_MESSAGE(action, value, message) \
    ASSERT_GREATER_EQUAL_MESSAGE_CALLBACK(action, value, message, {})

#define ASSERT_GREATER_EQUAL_CALLBACK(action, value, callback) \
    ASSERT_GREATER_EQUAL_MESSAGE_CALLBACK(action, value, NULL, callback)

#define ASSERT_GREATER_EQUAL(action, value) \
    ASSERT_GREATER_EQUAL_MESSAGE_CALLBACK(action, value, NULL, {})

#endif /* asserts.h */
