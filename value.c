#include <stdio.h>
#include <string.h>

#include "object.h"
#include "memory.h"
#include "value.h"

void initValueArray(ValueArray* array) {
    array->capacity = 0;
    array->count = 0;
    array->values = NULL;
}

void writeValueArray(ValueArray* array, Value value) {
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values = GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
    }
    array->values[array->count] = value;
    array->count++;
}

void freeValueArray(ValueArray* array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    initValueArray(array);
}

void printValue(Value value) {
    switch(value.type) {
        case VALUE_NIL:
            printf("nil"); break;
        case VALUE_BOOL:
            printf(AS_BOOL(value) ? "true" : "false"); break;
        case VALUE_NUMBER:
            printf("%g", AS_NUMBER(value)); break;
        case VALUE_OBJ:
            printObject(value);
    }
}

/**
 * @brief This function is meant to serve as a utility function that checks equality of values
 * This function is mainly meant to ensure that values of different types can be compared
 * For instance, comparing true == false is easy, since both values are boolean
 * Comparing 2 == true is harder, however, because both values are of different types
 * 
 * @param a 
 * @param b 
 * @return true 
 * @return false 
 */
bool valuesEqual(Value a, Value b) {
    if (a.type != b.type) return false;
    switch (a.type) {
        case VALUE_NIL:
            return false;
        case VALUE_BOOL:
            return AS_BOOL(a) == AS_BOOL(b);
        case VALUE_NUMBER:
            return AS_NUMBER(a) == AS_NUMBER(b);
        case VALUE_OBJ: {
            return AS_OBJ(a) == AS_OBJ(b);
        }
        default:
            return false;
    }
}