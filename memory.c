#include <stdlib.h>

#include "memory.h"
#include "vm.h"
#include "object.h"
#include "value.h"
#include "compiler.h"

#ifdef DEBUG_LOG_GC
#include <stdio.h>
#include "debug.h"
#endif

#define GC_HEAP_GROW_FACTOR 2;


void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    vm.bytesAllocated += newSize - oldSize;
    if (newSize > oldSize) {
        #ifdef DEBUG_STRESS_GC
            collectGarbage();
        #endif
        
        if (vm.bytesAllocated > vm.nextGC) {
            collectGarbage();
        }
    }
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }
    void* result = realloc(pointer, newSize);
    if (result == NULL) exit(1);
    return result;
}

void markObject(Obj* object) {
    if (object == NULL) return;
    if (object->isMarked) return;
    object->isMarked = true;

    if (vm.grayCount + 1 > vm.grayCapacity) {
        vm.grayCapacity = GROW_CAPACITY(vm.grayCapacity);
        vm.grayStack = (Obj**)realloc(vm.grayStack, sizeof(Obj*) * vm.grayCapacity);
        if (vm.grayStack == NULL) exit(1);
    }

    vm.grayStack[vm.grayCount++] = object;

    #ifdef DEBUG_LOG_GC
        printf("%p mark ", (void*)object);
        printValue(OBJ_VAL(object));
        printf("\n");
    #endif
}

void markValue(Value value) {
    if (IS_OBJ(value)) {
        markObject(AS_OBJ(value));
    }
}

static void markArray(ValueArray array) {
    Value* valuesArray = array.values;
    for (int i = 0; i < array.count; i++) {
        markValue(valuesArray[i]);
    }
}

static void blackenObject(Obj* object) {
    #ifdef DEBUG_LOG_GC
        printf("%p blacken ", (void*)object);
        printValue(OBJ_VAL(object));
        printf("\n");
    #endif
    switch(object->type) {
        case OBJ_STRING:
        case OBJ_NATIVE:
            break;
        case OBJ_UPVALUE:
            markValue(((ObjUpvalue*) object)->closed);
            break;
        case OBJ_FUNCTION: {
            ObjFunction* function = (ObjFunction*) object;
            markObject((Obj*) function->name);
            markArray(function->chunk.constants);
            break;
        }
        case OBJ_CLOSURE: {
            ObjClosure* closure = (ObjClosure*) object;
            ObjFunction* function = closure->function;
            markObject((Obj*) function);
            for (int i = 0; i < closure->upvalueCount; i++) {
                markObject((Obj*) closure->upvalues[i]);
            }
            break;
        }
    }
}

static void traceReferences() {
    while(vm.grayCount > 0) {
        Obj* object = vm.grayStack[--vm.grayCount];
        blackenObject(object);
    }
}

static void markRoots() {
    for(Value* slot = vm.stack; slot < vm.stackTop; slot++) {
        markValue(*slot);
    }

    for(int i = 0; i < vm.frameCount; i++) {
        markObject((Obj*)vm.frames[i].closure);
    }

    for(ObjUpvalue* upvalue = vm.openUpvalues; upvalue != NULL; upvalue = upvalue->next) {
        markObject((Obj*)upvalue);
    }

    markTable(&vm.globals);
    markCompilerRoots();
}

void freeObject(Obj* object) {
    #ifdef DEBUG_LOG_GC
        printf("%p free type %d\n", (void*)object, object->type);
    #endif
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

    free(vm.grayStack);
}

static void sweep() {
    Obj* previous = NULL;
    Obj* object = vm.objects;
    while (object != NULL) {
        if (object->isMarked == true) {
            object->isMarked = false;
            //  don't remove this object
            previous = object;
            object = object->next;
        } else {
            //  remove this object
            Obj* objectToRemove = object;
            object = object->next;
            if (previous != NULL) {
                //  not at head of the list
                previous->next = object;
            } else {
                //  at the head of the list
                vm.objects = object;
            }

            freeObject(objectToRemove);
        }
    }
}


void collectGarbage() {
    #ifdef DEBUG_LOG_GC
        printf("-- gc begin\n");
        size_t before = vm.bytesAllocated;
    #endif
    markRoots();
    traceReferences();
    tableRemoveWhites(&vm.strings);
    sweep();
    vm.nextGC = vm.bytesAllocated * GC_HEAP_GROW_FACTOR;
    #ifdef DEBUG_LOG_GC
        printf("-- gc end\n");
        printf("   collected %zu bytes (from %zu to %zu) next at %zu\n",
         before - vm.bytesAllocated, before, vm.bytesAllocated,
         vm.nextGC);
    #endif
}
