#include <string.h>
#include <stdlib.h>

#include "../unity.h"
#include "../table.h"

Table table;
ObjString* objString;
Key* key;
as value;
char string[] = "Hello World";

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

void test_function_insert_key_into_table(void) {
  objString = malloc(sizeof(*objString));
  objString->chars = string;
  objString->length = strlen(string);
  objString->hash = hashString(objString->chars, objString->length);
  as value = { .string = objString };
  key = ALLOCATE_KEY(KEY_STRING, value);
  bool result = tableSet(&table, key, NIL_VAL);
  TEST_ASSERT_TRUE(result == true);
}

void test_function_get_key_from_table(void) {
  bool result = tableGet(&table, key, &NIL_VAL);
  TEST_ASSERT_TRUE(result);
}

void test_function_find_entry_in_table(void) {
  Entry* entry = findEntry(table.entries, table.capacity, key);
  switch (entry->key->type) {
    case KEY_STRING:
      TEST_ASSERT_EQUAL_STRING(string, entry->key->as.string->chars);
      break;
    case KEY_BOOL:
      break;
    case KEY_NUMBER:
      break;
    case KEY_NIL:
      break;
  }
}

void test_function_delete_key_from_table(void) {
  bool result = tableDelete(&table, key);
  TEST_ASSERT_TRUE(result);
}

void test_function_get_key_from_table_after_delete(void) {
  bool result = tableGet(&table, key, &NIL_VAL);
  TEST_ASSERT_FALSE(result);
}

// not needed when using generate_test_runner.rb
int main(void) {
  initTable(&table);
  UNITY_BEGIN();
  RUN_TEST(test_function_insert_key_into_table);
  RUN_TEST(test_function_get_key_from_table);
  RUN_TEST(test_function_find_entry_in_table);
  RUN_TEST(test_function_delete_key_from_table);
  RUN_TEST(test_function_get_key_from_table_after_delete);
  free(objString);
  free(key);
  return UNITY_END();
}