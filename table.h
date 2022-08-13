#include "object.h"
#include "value.h"

typedef enum KeyType {
  KEY_BOOL,
  KEY_NUMBER,
  KEY_NIL,
  KEY_STRING
} KeyType;

typedef union {
  bool boolean;
  int number;
  ObjString* string;
} as;

typedef struct Key {
  KeyType type;
  union {
    bool boolean;
    int number;
    ObjString* string;
  } as;
} Key;

typedef struct Entry {
  // ObjString* key;
  Key* key;
  Value value;
} Entry;

typedef struct Table {
  int count;
  int capacity;
  Entry* entries;
} Table;

#define ALLOCATE_KEY(keyType, value) (Key*)allocateKey(keyType, value)

Key* allocateKey(KeyType keyType, as value);
void printEntry(Entry* entry);

void initTable(Table* table);
bool tableGet(Table* table, Key* key, Value* value);
bool tableSet(Table* table, Key* key, Value value);
bool tableDelete(Table* table, Key* key);
void tableAddAll(Table* from, Table* to);
Entry* findEntry(Entry* entries, int capacity, Key* key);
ObjString* tableFindString(Table* table, const char* chars, int length, uint32_t hash);
void freeTable(Table* table);
