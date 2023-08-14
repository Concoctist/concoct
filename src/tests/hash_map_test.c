#include "hash_map.h"

#include <stdio.h>

#define KEYWORD_COUNT 23
#define HASH_MAP_BUCKET_COUNT 24

const char* cct_keywords[KEYWORD_COUNT] = {
  "break",
  "continue",
  "case",
  "class",
  "do",
  "default",
  "else",
  "enum",
  "false",
  "for",
  "func",
  "goto",
  "if",
  "namespace",
  "null",
  "return",
  "switch",
  "super",
  "true",
  "use",
  "var",
  "while",
  "in"
};

int main()
{
  ConcoctHashMap* map = cct_new_hash_map(HASH_MAP_BUCKET_COUNT);
  // Print out the results of hashing
  printf("\nHASH MAP TEST\n\n");
  printf("%-9s : %-10s : %s\n\n", "KEY", "HASH_CODE", "BUCKET_INDEX");
  for(int i = 0; i < KEYWORD_COUNT; i++)
  {
    const char* key = cct_keywords[i];
    unsigned int hash = cct_get_hash_code(key);
    int bucket = hash % HASH_MAP_BUCKET_COUNT;
    printf("%-9s : %10u : %d\n", key, hash, bucket);
  }
  printf("\n");

  // Some hash map tests
  cct_hash_map_set(map, "var", NULL);
  cct_hash_map_set(map, "thisIsACrazyLongString", NULL);
  cct_hash_map_set(map, "deleteMe", NULL);
  cct_hash_map_delete_entry(map, "deleteMe");
  cct_hash_map_set(map, "five", (void*)5);

  if(!cct_hash_map_has_key(map, "var"))
  {
    printf("TEST 1 FAILED: Error while setting and testing a hash map key.\n");
  }
  else if(!cct_hash_map_has_key(map, "thisIsACrazyLongString"))
  {
    printf("TEST 2 FAILED: Error while setting and testing a hash map key.\n");
  }
  else if(cct_hash_map_has_key(map, "thisKeyDoesNotExist"))
  {
    printf("TEST 3 FAILED: Error while testing for key that does not exist.\n");
  }
  else if(cct_hash_map_has_key(map, "deleteMe"))
  {
    printf("TEST 4 FAILED: Error while testing for key that was deleted.\n");
  }
  else if(cct_hash_map_get(map, "five") != (void*)5)
  {
    printf("TEST 5 FAILED: Error while retrieving value\n");
  }
  else
  {
    printf("ALL TESTS PASSED\n");
  }
  cct_delete_hash_map(map);
}