#include <stdio.h>
#include <string.h>

#include "object.h"
#include "memory.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) (type*)allocateObject(sizeof(type), objectType)

#define ALLOCATE_OBJ_WITH_FLEXIBLE_ARRAY_MEMBER(structType, arrayType, arrayLength, objectType) \
    (structType*)allocateObject(sizeof(structType) + arrayLength * sizeof(arrayType), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
    Obj* object = (Obj*)reallocate(NULL, 0, size);
    object->type = type;
    object->next = vm.objects;
    object->refCount = 0;
    vm.objects = object;
    return object;
}

ObjString* allocateString(char* chars, int length, uint32_t hash) {
    ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    // ObjString* string = ALLOCATE_OBJ_WITH_FLEXIBLE_ARRAY_MEMBER(ObjString, char, length, OBJ_STRING);
    string->length = length;
    string->chars = chars;
    string->hash = hash;
    tableSet(&vm.strings, string, NIL_VAL);
    // strcpy(string->chars, chars);
    return string;
}

static uint32_t hashString(const char* key, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }
  return hash;
}

ObjUpvalue* newUpvalue(Value* slot) {
    ObjUpvalue* upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);
    upvalue->location = slot;
    upvalue->next = NULL;
    upvalue->closed = NIL_VAL;
    return upvalue;
}

ObjFunction* newFunction() {
    ObjFunction* function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
    function->arity = 0;
    function->upvalueCount = 0;
    function->name = NULL;
    initChunk(&function->chunk);
    return function;
}

ObjClosure* newClosure(ObjFunction* function) {
    ObjUpvalue** upvalues = ALLOCATE(ObjUpvalue*, function->upvalueCount);
    for (int i = 0; i < function->upvalueCount; i++) {
        upvalues[i] = NULL;
    }
    ObjClosure* closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
    closure->function = function;
    closure->upvalues = upvalues;
    closure->upvalueCount = function->upvalueCount;
    return closure;
}

ObjNative* newNative(NativeFn function, int arity) {
    ObjNative* native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
    native->arity = arity;
    native->function = function;
    return native;
}

/**
 * @brief This method is used when a copy of the original string does not need to be created.
 * It is mainly used while concatenating strings together in the VM
 * 
 * @param chars 
 * @param length 
 * @return ObjString* 
 */
ObjString* takeString(char* chars, int length) {
    uint32_t hash = hashString(chars, length);
    ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
    if (interned != NULL) {
        FREE_ARRAY(char, chars, length + 1);
        return interned;
    }
    return allocateString(chars, length, hash);
}

/**
 * @brief This method is used when copying a string found by the compiler in source code
 * onto the heap allocated by the vm.
 * A copy of the string is created onto the heap
 * 
 * @param chars 
 * @param length 
 * @return ObjString* 
 */
ObjString* copyString(const char* chars, int length) {
    uint32_t hash = hashString(chars, length);
    ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
    if (interned != NULL) {
        return interned;
    }
    char* heapChars = ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return allocateString(heapChars, length, hash);
}

static void printFunction(ObjFunction* function) {
    if (function->name == NULL) {
        printf("<script>");
        return;
    }
    printf("<fn %s>", function->name->chars);
}

void printObject(Value value) {
    switch(OBJ_TYPE(value)) {
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
        case OBJ_FUNCTION:
            printFunction(AS_FUNCTION(value));
            break;
        case OBJ_NATIVE:
            printf("<native fn>");
            break;
        case OBJ_CLOSURE:
            printFunction(AS_CLOSURE(value)->function);
            break;
        case OBJ_UPVALUE:
            printf("upvalue");
            break;
        default:
            fprintf(stderr, "Object is of unknown type");
    }
}

void handleRefCount(ObjString* name, Value value) {
    uint32_t hash = hashString(name->chars, name->length);
    ObjString* returnValue = tableFindString(&vm.globals, name->chars, name->length, hash);
    if (returnValue != NULL) {
        //  this variable currently references some other object. Get the object it references, decrement its ref counter
        Entry* entry = findEntry(vm.globals.entries, vm.globals.capacity, name);
        Value oldValue = entry->value;
        if (IS_OBJ(oldValue) == true) {
            decrementObjectRefCount(AS_OBJ(oldValue));
        }
    }
    if (IS_OBJ(value) == true) {
        //  now increment the ref counter of the new object the variable will reference
        //  but only if it the new value is an Object
        incrementObjectRefCount(AS_OBJ(value));
    }
}