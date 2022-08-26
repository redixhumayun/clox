#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "value.h"
#include "chunk.h"

//  These functions will convert from CLox values to native C values
#define OBJ_TYPE(value) (AS_OBJ(value)->type)

//  This function will check if the value provided is of type OBJ_STRING
#define IS_STRING(value) isObjType(value, OBJ_STRING)
#define IS_FUNCTION(value) isObjType(value, OBJ_FUNCTION)
#define IS_NATIVE(value) isObjType(value, OBJ_NATIVE)

//  These functions will convert Value structs to ObjString or ObjFunction structs
#define AS_FUNCTION(value) ((ObjFunction*)(AS_OBJ(value)))
#define AS_STRING(value) ((ObjString*)(AS_OBJ(value)))
#define AS_CSTRING(value) (((ObjString*)(AS_OBJ(value)))->chars)
#define AS_NATIVE(value) ((ObjNative*)(AS_OBJ(value)))->function

typedef enum {
    OBJ_FUNCTION,
    OBJ_STRING,
    OBJ_NATIVE
} ObjType;

struct Obj {
    ObjType type;
    struct Obj* next;
};

typedef struct {
    Obj obj;
    int arity;
    Chunk chunk;
    ObjString* name;
} ObjFunction;

typedef Value (*NativeFn)(int argCount, Value* args);
typedef struct  {
    Obj obj;
    NativeFn function;
}ObjNative;

struct ObjString {
    Obj obj;
    int length;
    char* chars;
    uint32_t hash;
    // char chars[];
};

ObjFunction* newFunction();
ObjNative* newNative(NativeFn function);
ObjString* takeString(char* chars, int length);
ObjString* copyString(const char* chars, int length);
void printObject(Value value);
ObjString* allocateString(char* chars, int length, uint32_t hash);

//  This function will check if the value provided has the same type as the parameter
static inline bool isObjType(Value value, ObjType type) {
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif