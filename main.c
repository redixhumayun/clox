#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

void repl() {
  char line[1024];
  for(;;) {
    printf("> ");
    if (fgets(line, sizeof(line), stdin) == NULL) {
      printf("Error");
      break;
    } 

    interpret(line);
  }
}

static char* readFile(const char* path) {
  FILE* file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    exit(74);
  }

  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  char* buffer = (char*)malloc(fileSize + 1);
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
    exit(74);
  }
  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  if (bytesRead < fileSize) {
    fprintf(stderr, "Could not read the entire file \"%s\".\n", path);
    exit(74);
  }
  buffer[bytesRead] = '\0';

  fclose(file);
  return buffer;
}

void runFile(const char* path) {
  char* source = readFile(path);
  InterpretResult result = interpret(source);
  free(source);

  if (result == INTERPRET_COMPILE_ERROR) exit(65);
  if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

// int main(int argc, const char* argv[]) {
//   initVM();

//   if (argc == 1) {
//     repl();
//   } else if (argc == 2) {
//     runFile(argv[1]);
//   } else {
//     fprintf(stderr, "Usage: clox [PATH]");
//     exit(64);
//   }

//   freeVM();
//   return 0;
// }

int main (int argc, char* argv[]) {
  //  This main function is currently being used to test out the implementation of the hash table
  clock_t begin = clock();
  Table table;
  initTable(&table);
  Obj* a = (Obj*)"Hello World!";
  ObjString* aString = (ObjString*) a;
  tableSet(&table, aString, NIL_VAL);
  bool ret = tableGet(&table, aString, &NIL_VAL);
  if (ret == true) {
    printf("Found the string\n");
  } else {
    printf("Could not find the string\n");
  }
  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("%f\n", time_spent);
  return 0;
}