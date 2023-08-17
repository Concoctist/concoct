/*
 * Concoct - An imperative, dynamically-typed, interpreted, general-purpose programming language
 * Copyright (c) 2020-2023 BlakeTheBlock and Lloyd Dilley
 * http://concoct.ist/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>    // printf()
#include "debug.h"    // debug_mode, debug_print()
#include "hash_map.h"

#define KEYWORD_COUNT ((uint8_t)23)
#define HASH_MAP_BUCKET_COUNT ((uint16_t)24)

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
  debug_mode = true;
  ConcoctHashMap* map = cct_new_hash_map(HASH_MAP_BUCKET_COUNT);

  // Print out the results of hashing
  printf("\nHASH MAP TEST\n\n");
  printf("%-9s : %-10s : %s\n\n", "KEY", "HASH_CODE", "BUCKET_INDEX");

  for(size_t i = 0; i < KEYWORD_COUNT; i++)
  {
    const char* key = cct_keywords[i];
    uint32_t hash = cct_get_hash_code(key);
    uint32_t bucket = hash % HASH_MAP_BUCKET_COUNT;
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
    printf("TEST 1 FAILED: Error while setting and testing a hash map key.\n");
  else if(!cct_hash_map_has_key(map, "thisIsACrazyLongString"))
    printf("TEST 2 FAILED: Error while setting and testing a hash map key.\n");
  else if(cct_hash_map_has_key(map, "thisKeyDoesNotExist"))
    printf("TEST 3 FAILED: Error while testing for key that does not exist.\n");
  else if(cct_hash_map_has_key(map, "deleteMe"))
    printf("TEST 4 FAILED: Error while testing for key that was deleted.\n");
  else if(cct_hash_map_get(map, "five") != (void*)5)
    printf("TEST 5 FAILED: Error while retrieving value\n");
  else
    printf("ALL TESTS PASSED\n");

  cct_delete_hash_map(map);
}
