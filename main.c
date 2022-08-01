#include <stdio.h>

#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, const char* argv[]) {
  Chunk chunk;
  initChunk(&chunk);
  int constant = addConstant(&chunk, 1.2);
  writeChunk(&chunk, OP_CONSTANT, 1);
  writeChunk(&chunk, constant, 1);
  writeChunk(&chunk, OP_RETURN, 2);
  for (int i = 0; i < chunk.count; i++) {
    printf("Index - %d, Value - %d", i, chunk.lines[i]);
    printf("\n");
  }
  disassembleChunk(&chunk, "test chunk");
  freeChunk(&chunk);
  return 0;
}