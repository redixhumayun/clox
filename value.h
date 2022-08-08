#ifndef clox_value_h
#define clox_value_h

#include "common.h"

typedef enum {
    VALUE_BOOL,
    VALUE_NIL,
    VALUE_NUMBER
} ValueType;

typedef struct {
    ValueType type;
    union {
        bool boolean;
        double number;
    } as;
} Value;

//  This set of functions is used as guard checks to make sure that the values are what they say they are
#define IS_BOOL(value) ((value).type == VALUE_BOOL)
#define IS_NIL(value) ((value).type == VALUE_NIL)
#define IS_NUMBER(value) ((value).type == VALUE_NUMBER)

//  This set of functions takes some CLox values and unwraps them into native C values
#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)

//  This set of functions takes native C values and wraps them into CLox Value structs
#define BOOL_VAL(value) ((Value) {VALUE_BOOL, {.boolean = value}})
#define NIL_VAL ((Value) {VALUE_NIL, {.number = 0}})
#define NUMBER_VAL(value) ((Value) {VALUE_NUMBER, {.number = value}})

typedef struct {
    int capacity;
    int count;
    Value* values;
} ValueArray;

void initValueArray(ValueArray* array);
void writeValueArray(ValueArray* array, Value value);
void freeValueArray(ValueArray* array);
void printValue(Value value);
bool valuesEqual(Value a, Value b);

#endif