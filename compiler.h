#ifndef clox_compiler_h
#define clox_compiler_h

#include "chunk.h"
#include "object.h"
#include "memory.h"

ObjFunction* compile(const char* source);
void markCompilerRoots();

#endif