#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "memory.h"
#include "table.h"
#include "object.h"
#include "value.h"

#define TABLE_MAX_LOAD 0.75

Key* allocateKey(KeyType keyType, as value) {
  size_t size = sizeof(keyType);
  Key* key = (Key*)reallocate(NULL, 0, size);
  key->type = keyType;
  switch (keyType) {
    case KEY_BOOL:
      key->as.boolean = value.boolean;
      break;
    case KEY_NUMBER:
      key->as.number = value.number;
      break;
    case KEY_STRING:
    key->as.string = value.string;
      break;
    default:
      fprintf(stderr, "Could not identify this key type\n");
      exit(1);
  }
  return key;
}

static void printKey(Key* key) {
  switch(key->type) {
    case KEY_BOOL:
      printf("%s\n", key->as.boolean ? "true" : false);
      break;
    case KEY_NUMBER:
      printf("%d\n", key->as.number);
      break;
    case KEY_STRING:
      printf("%s\n", key->as.string->chars);
      break;
    case KEY_NIL:
      break;
    default:
      fprintf(stderr, "Unknown key type");
      break;
  }
}

void printEntry(Entry* entry) {
  Key* key = entry->key;
  printKey(key);
}

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

Entry* findEntry(Entry* entries, int capacity, Key* key) {
  uint32_t tableIndex;
  // uint32_t tableIndex = key->hash % capacity;
  switch (key->type) {
    case KEY_BOOL:
      tableIndex = key->as.boolean == false ? 0 % capacity : 1 % capacity;
      break;
    case KEY_NUMBER:
      tableIndex = key->as.number % capacity;
      break;
    case KEY_STRING:
      tableIndex = key->as.string->hash % capacity;
      break;
    case KEY_NIL:
      fprintf(stderr, "This case is not handled yet\n");
      break;
    default:
      fprintf(stderr, "This key type is unknown\n");
  }
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

bool tableSet(Table* table, Key* key, Value value) {
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
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

bool tableGet(Table* table, Key* key, Value* value) {
  if (table->capacity == 0) return false;

  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL) return false;

  *value = entry->value;
  return true;
}

bool tableDelete(Table* table, Key* key) {
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
    } 
    else {
      switch (entry->key->type) {
        case KEY_STRING:
          if (entry->key->as.string->length == length &&
              entry->key->as.string->hash == hash &&
              memcmp(entry->key->as.string->chars, chars, length) == 0) {
                return entry->key->as.string;
              }
        case KEY_BOOL:
          break;
        case KEY_NIL:
          break;
        case KEY_NUMBER:
          break;
      }
    }
    // else if (entry->key->length == length && 
    //             entry->key->hash == hash && 
    //             memcmp(entry->key->chars, chars, length) == 0) {
    //               return entry->key;
    //             }
    index = (index + 1) % table->capacity;
  }
}

void freeTable(Table* table) {
  FREE_ARRAY(Entry, table, table->count);
  initTable(table);
}