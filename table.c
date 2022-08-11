#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "table.h"
#include "object.h"
#include "memory.h"

void initTable(Table* table) {
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
}

void adjustCapacity(Table* table, int capacity) {
  Entry* newEntries = ALLOCATE(Entry, capacity);
  for(int i = 0; i < capacity; i++) {
    newEntries[i].key = NULL;
    newEntries[i].value = NIL_VAL;
  }

  table->count = 0;
  //  copy over values from the previous entries table into the new entries table but recalculate the index for each entry
  for(int i = 0; i < table->capacity; i++) {
    Entry* entry = &table->entries[i];
    if (entry->key == NULL) continue;  //  nothing to copy here
    
    Entry* dest = findEntry(newEntries, capacity, entry->key);
    dest->key = entry->key;
    dest->value = entry->value;
    table->count++;
  }

  FREE_ARRAY(Entry, table->entries, table->capacity);
  table->entries = newEntries;
  table->capacity = capacity;
}

Entry* findEntry(Entry* entries, int capacity, ObjString* key) {
  uint32_t tableIndex = key->hash % capacity;
  Entry* tombstone = NULL;

  for(;;) {
    Entry* entry = &entries[tableIndex];
    if (entry->key == NULL) {
      if (IS_NIL(entry->value)) {
        //  found an empty value. Check if the previous iteration caused a tombstone to be set
        return tombstone != NULL ? tombstone : entry;
      } else {
        //  found a tombstone
        if (tombstone == NULL) tombstone = entry; 
      }
    }else if (entry->key == key) {
      return entry;
    }
    tableIndex = (tableIndex + 1) % capacity;
  }
}

bool tableSet(Table* table, ObjString* key, Value value) {
  if (table->count + 1 > table->capacity) {
    int capacity = GROW_CAPACITY(table->capacity);
    adjustCapacity(table, capacity);
  }
  Entry* entries = table->entries;
  Entry* entry = findEntry(entries, table->capacity, key);
  bool isNewKey = entry->key == NULL;
  if (isNewKey && IS_NIL(entry->value)) table->count++;

  entry->key = key;
  entry->value = value;
  return isNewKey;
}

bool tableGet(Table* table, ObjString* key, Value* value) {
  if (table->capacity == 0) return false;

  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL) return false;

  *value = entry->value;
  return true;
}

bool tableDelete(Table* table, ObjString* key) {
  if (table->count == 0) return false;
  Entry* entries = table->entries;
  Entry* entry = findEntry(entries, table->capacity, key);
  if (entry->key == NULL) {
    //  this key does not exist
    return false;
  }

  //  create a tombstone value
  entry->key = NULL;
  entry->value = BOOL_VAL(true);
  return true;
}

void tableAddAll(Table* from, Table* to) {
  for (int i = 0; i < from->capacity; i++) {
    Entry* entry = &from->entries[i];
    if (entry->key != NULL) {
      tableSet(to, entry->key, entry->value);
    }
  }
}

ObjString* tableFindString(Table* table, const char* chars, int length, uint32_t hash) {
  if (table->count == 0) return NULL;

  uint32_t index = hash % table->capacity;
  Entry* entries = table->entries;

  for (;;) {
    Entry* entry = &entries[index];
    if (entry->key == NULL) {
      if (IS_NIL(entry->value)) {
        return NULL;
      }
    } else if (entry->key->length == length && 
                entry->key->hash == hash && 
                memcmp(entry->key->chars, chars, length) == 0) {
                  return entry;
                }
    index = (index + 1) % table->capacity;
  }
}

void freeTable(Table* table) {
  FREE_ARRAY(Entry, table, table->count);
  initTable(table);
}