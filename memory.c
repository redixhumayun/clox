#include <stdlib.h>
#include "memory.h"
#include "vm.h"
#include "object.h"
#include "value.h"

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }
    void* result = realloc(pointer, newSize);
    if (result == NULL) exit(1);
    return result;
}

void freeObject(Obj* object) {
    switch(object->type) {
        case OBJ_STRING: {
            ObjString* s = (ObjString*)object;
            FREE_ARRAY(char, s->chars, s->length + 1);
            FREE(ObjString, object); 
            break;
        }
    }
}

void freeObjects() {
    Obj* object = vm.objects;
    while (object != NULL) {
        Obj* next = object->next;
        freeObject(object);
        object = next;
    }
}

