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

void printObject(Value value) {
    switch(OBJ_TYPE(value)) {
        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;
        default:
            fprintf(stderr, "Object is of unknown type");
    }
}