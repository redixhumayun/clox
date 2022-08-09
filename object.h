#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "value.h"

//  These functions will convert from CLox values to native C values
#define OBJ_TYPE(value) (AS_OBJ(value)->type)

//  This function will check if the value provided is of type OBJ_STRING
#define IS_STRING(value) isObjType(value, OBJ_STRING)

//  These functions will convert Value structs to ObjString structs
#define AS_STRING(value) ((ObjString*)(AS_OBJ(value)))
#define AS_CSTRING(value) (((ObjString*)(AS_OBJ(value)))->chars)

typedef enum {
    OBJ_STRING
} ObjType;

struct Obj {
    ObjType type;
    struct Obj* next;
};

struct ObjString {
    Obj obj;
    int length;
    // char* chars;
    char chars[];
};

ObjString* copyString(const char* chars, int length);
void printObject(Value value);
ObjString* allocateString(char* chars, int length);

//  This function will check if the value provided has the same type as the parameter
static inline bool isObjType(Value value, ObjType type) {
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif