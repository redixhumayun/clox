#include <stdlib.h>

#include "memory.h"
#include "vm.h"
#include "object.h"
#include "value.h"

#ifdef DEBUG_LOG_GC
#include <stdio.h>
#endif

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    vm.bytesAllocated += newSize - oldSize;
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
        case OBJ_FUNCTION: {
            ObjFunction* function = (ObjFunction*)object;
            freeChunk(&function->chunk);
            FREE(ObjFunction, object);
            break;
        }
        case OBJ_NATIVE: {
            ObjNative* function = (ObjNative*)object;
            FREE(ObjNative, object);
            break;
        }
        case OBJ_CLOSURE: {
            ObjClosure* closure = (ObjClosure*)object;
            FREE_ARRAY(ObjUpvalue*, closure->upvalues, closure->upvalueCount);
            FREE(ObjClosure, object);
            break;
        }
        case OBJ_UPVALUE: {
            FREE(ObjUpvalue, object);
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

static void removeObjectFromStringsTable(Obj* object) {
    #ifdef DEBUG_LOG_GC
        printf("%p Removing this object from the strings table: ", (void*)object);
        printValue(OBJ_VAL(object));
        printf("\n");
    #endif
    bool result = tableDelete(&vm.strings, AS_STRING(OBJ_VAL(object)));
    #ifdef DEBUG_LOG_GC
        if (result == 1) {
            printf("%p Removed the object from the strings table\n", (void*)object);
        } else {
            printf("%p Failed to remove the object because it was not found in the table\n", (void*)object);
        }
    #endif
}

static void removeObjectFromGlobalsTable(Obj* object) {
    #ifdef DEBUG_LOG_GC
        printf("%p Removing this object from the globals table: ", (void*)object);
        printValue(OBJ_VAL(object));
        printf("\n");
    #endif
    bool result = tableDelete(&vm.globals, AS_STRING(OBJ_VAL(object)));
    #ifdef DEBUG_LOG_GC
        if (result == 1) {
            printf("%p Removed the object from the globals table\n", (void*)object);
        } else {
            printf("%p Failed to remove the object because it was not found in the table\n", (void*)object);
        }
    #endif
}

static void removeObjectFromVMList(Obj* object) {
    Obj* previous = NULL;
    Obj* objectHead = vm.objects;
    while (objectHead != NULL) {
        if (object == objectHead) {
            objectHead = objectHead->next;
            if (previous != NULL) {
                //  not at the head of the list
                previous->next = objectHead;
            } else {
                //  at the head of the list
                vm.objects = objectHead;
            }
            freeObject(object);
        } else {
            previous = objectHead;
            objectHead = objectHead->next;
        }
    }
}

static void removeObjectReferences(Obj* object) {
    switch(object->type) {
        case OBJ_STRING:
            removeObjectFromStringsTable(object);
            break;
        case OBJ_FUNCTION: {
            ObjFunction* function = (ObjFunction*) object;
            removeObjectFromGlobalsTable((Obj*)function->name);
            decrementObjectRefCount((Obj*)function->name);
            //  decrement ref count for all heap allocated objects in the constants table for the function
            for(int i = 0; i < function->chunk.constants.capacity; i++) {
                Value value = function->chunk.constants.values[i];
                if (IS_OBJ(value)) {
                    decrementObjectRefCount(AS_OBJ(value));
                }
            }
            break;
        }
        case OBJ_CLOSURE: {
            ObjClosure* closure = (ObjClosure*) object;
            removeObjectReferences((Obj*) closure->function);
            //  decrement ref count for all heap allocated objects in the upvalues list of the closure
            // for(int i = 0; i < closure->upvalueCount; i++) {
            //     Value value = *(closure->upvalues[i]->location);
            //     if (IS_OBJ(value)) {
            //         decrementObjectRefCount(AS_OBJ(value));
            //     }
            // }
            break;
        }
        case OBJ_UPVALUE:
            break;
        case OBJ_NATIVE:
            break;
    }
}

void incrementObjectRefCount(Obj* object) {
    #ifdef DEBUG_LOG_GC
        printf("%p Incrementing the value of the counter for: ", (void*)object);
        printValue(OBJ_VAL(object));
        printf(" from %d to %d\n", object->refCount, object->refCount + 1);
    #endif
    object->refCount++;
    return;
}

void decrementObjectRefCount(Obj* object) {
    #ifdef DEBUG_LOG_GC
        printf("%p Decrementing the value of the counter for: ", (void*)object);
        printValue(OBJ_VAL(object));
        printf(" from %d to %d\n", object->refCount, object->refCount - 1);
    #endif
    if (object->refCount == 0) {
        //  something went wrong. decrementing shouldn't be possible for an object with no references
        exit(1);
        return;
    }
    object->refCount--;
    
    if (object->refCount == 0) {
        #ifdef DEBUG_LOG_GC
            printf("-- gc begin\n");
            printf("%p Freeing the object: ", (void*)object);
            printValue(OBJ_VAL(object));
            printf("\n");
            size_t before = vm.bytesAllocated;
        #endif
        //  remove the object from the &vm.strings table here before removing from vm list
        removeObjectReferences(object);
        removeObjectFromVMList(object);
        #ifdef DEBUG_LOG_GC
            printf("Collected %zu bytes (from %zu to %zu)\n", before - vm.bytesAllocated, before, vm.bytesAllocated);
            printf("-- gc end\n");
        #endif
    }
}

/**
 * NOTE: When removing allocated strings because their ref count has fallen to zero, they also 
 * need to be removed from the &vm.strings table. Otherwise there will be a dangling pointer left in the 
 * &vm.strings table which points to undefined memory since that memory has been freed.
 */
