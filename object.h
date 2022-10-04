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
#define IS_CLOSURE(value) isObjType(value, OBJ_CLOSURE)

//  These functions will convert Value structs to ObjString or ObjFunction structs
#define AS_FUNCTION(value) ((ObjFunction*)(AS_OBJ(value)))
#define AS_STRING(value) ((ObjString*)(AS_OBJ(value)))
#define AS_CSTRING(value) (((ObjString*)(AS_OBJ(value)))->chars)
#define AS_NATIVE_FUNCTION_OBJECT(value) ((ObjNative*)(AS_OBJ(value)))
#define AS_NATIVE(value) ((ObjNative*)(AS_OBJ(value)))->function
#define AS_CLOSURE(value) ((ObjClosure*)(AS_OBJ(value)))

typedef enum {
    OBJ_CLOSURE,
    OBJ_UPVALUE,
    OBJ_FUNCTION,
    OBJ_STRING,
    OBJ_NATIVE
} ObjType;

struct Obj {
    struct Obj* next;
    ObjType type;
    bool isMarked;
};

typedef struct {
    Obj obj;
    int arity;
    int upvalueCount;
    Chunk chunk;
    ObjString* name;
} ObjFunction;

typedef struct ObjUpvalue {
    Obj obj;
    Value* location;
    Value closed;
    struct ObjUpvalue* next;
} ObjUpvalue;

typedef struct {
    Obj obj;
    ObjFunction* function;
    ObjUpvalue** upvalues;
    int upvalueCount;
} ObjClosure;

typedef Value (*NativeFn)(int argCount, Value* args);
typedef struct  {
    Obj obj;
    int arity;
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
ObjClosure* newClosure(ObjFunction* function);
ObjUpvalue* newUpvalue(Value* slot);
ObjNative* newNative(NativeFn function, int arity);
ObjString* takeString(char* chars, int length);
ObjString* copyString(const char* chars, int length);
void printObject(Value value);
ObjString* allocateString(char* chars, int length, uint32_t hash);

//  This function will check if the value provided has the same type as the parameter
static inline bool isObjType(Value value, ObjType type) {
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

#endif