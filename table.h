#include "object.h"
#include "value.h"

typedef struct Entry {
  ObjString* key;
  Value value;
} Entry;

typedef struct Table {
  int count;
  int capacity;
  Entry* entries;
} Table;

void initTable(Table* table);
bool tableGet(Table* table, ObjString* key, Value* value);
bool tableSet(Table* table, ObjString* key, Value value);
bool tableDelete(Table* table, ObjString* key);
void tableAddAll(Table* from, Table* to);
Entry* findEntry(Entry* entries, int capacity, ObjString* key);
ObjString* tableFindString(Table* table, const char* chars, int length, uint32_t hash);
void markTable(Table* table);
void freeTable(Table* table);
void tableRemoveWhites(Table* table);
