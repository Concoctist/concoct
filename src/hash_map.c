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

#include <errno.h>    // errno
#include <inttypes.h> // PRIu32
#include <stdio.h>    // fprintf(), stderr
#include <stdlib.h>   // free(), malloc()
#include <string.h>   // strcmp(), strerror(), strlen()
#include "debug.h"    // debug_mode, debug_print()
#include "hash_map.h"

ConcoctHashMap* cct_new_hash_map(uint32_t bucket_count)
{
  ConcoctHashMap* map = malloc(sizeof(ConcoctHashMap));
  if(map == NULL)
  {
    fprintf(stderr, "Failed to allocate memory for a hash map: %s\n", strerror(errno));
    return NULL;
  }

  map->bucket_count = bucket_count;
  map->buckets = malloc(bucket_count * sizeof(ConcoctHashMapNode*));
  if(map->buckets == NULL)
  {
    fprintf(stderr, "Failed to allocate memory for hash map buckets: %s\n", strerror(errno));
    return NULL;
  }

  for(size_t i = 0; i < bucket_count; i++)
  {
    ConcoctHashMapNode** bucket = map->buckets + i;
    *bucket = NULL;
  }

  if(debug_mode)
    debug_print("Hash map created with %zu buckets.", bucket_count);

  return map;
}

void cct_delete_hash_map(ConcoctHashMap* map)
{
  for(size_t i = 0; i < map->bucket_count; i++)
  {
    if(map->buckets[i] != NULL)
    {
      cct_delete_hash_map_node(map->buckets[i]);
    }
  }
  free(map->buckets);
  free(map);

  if(debug_mode)
    debug_print("Freed hash map.");

  return;
}

ConcoctHashMapNode* cct_new_hash_map_node(const char* key, void* value, uint32_t hash)
{
  ConcoctHashMapNode* node = malloc(sizeof(ConcoctHashMapNode));
  if(node == NULL)
  {
    fprintf(stderr, "Failed to allocate memory for hash map key: %s\n", strerror(errno));
    return NULL;
  }

  node->hash = hash;
  node->key = key;
  node->value = value;
  node->next = NULL;

  if(debug_mode)
    debug_print("Hash map node created with key: %s (hash: %" PRIu32 ")", key, hash);

  return node;
}

void cct_delete_hash_map_node(ConcoctHashMapNode* node)
{
  if(node->next != NULL)
  {
    cct_delete_hash_map_node(node->next);
  }
  free(node);

  return;
}

bool cct_hash_map_has_key(const ConcoctHashMap* map, const char* key)
{
  // Finds the bucket that this key could be in and checks each entry in it
  uint32_t hash = cct_get_hash_code(key);
  uint32_t bucket_index = hash % map->bucket_count;
  ConcoctHashMapNode* node = map->buckets[bucket_index];

  while(node != NULL)
  {
    // Checks hash first for efficiency. Checks the string itself in case of hash collisions
    if(node->hash == hash && strcmp(node->key, key) == 0)
      return true;
    node = node->next;
  }

  return false;
}

void cct_hash_map_set(ConcoctHashMap* map, const char* key, void* value)
{
  unsigned int hash = cct_get_hash_code(key);
  int bucket_index = hash % map->bucket_count;
  ConcoctHashMapNode* node = map->buckets[bucket_index];

  if(node == NULL)
  {
    // Starts the first node in the bucket
    map->buckets[bucket_index] = cct_new_hash_map_node(key, value, hash);
  }
  else
  {
    // Traverses the linked list to find where to put new node
    while(node->next != NULL)
    {
      node = node->next;
    }
    node->next = cct_new_hash_map_node(key, value, hash);
  }

  return;
}

void* cct_hash_map_get(const ConcoctHashMap* map, const char* key)
{
  // Finds the bucket that this key could be in and checks each entry in it
  uint32_t hash = cct_get_hash_code(key);
  uint32_t bucket_index = hash % map->bucket_count;
  ConcoctHashMapNode* node = map->buckets[bucket_index];

  while(node != NULL)
  {
    // Checks hash first for efficiency. Checks the string itself in case of hash collisions
    if(node->hash == hash && strcmp(node->key, key) == 0)
    {
      return node->value;
    }
    node = node->next;
  }

  return NULL;
}

void cct_hash_map_delete_entry(ConcoctHashMap* map, const char* key)
{
  uint32_t hash = cct_get_hash_code(key);
  uint32_t bucket_index = hash % map->bucket_count;
  ConcoctHashMapNode* node = map->buckets[bucket_index];
  ConcoctHashMapNode* previous_node = NULL;

  while(node)
  {
    if(node->hash == hash && strcmp(node->key, key) == 0)
    {
      if(previous_node)
      {
        previous_node->next = node->next;
        // Must set to NULL or the delete function will free node->next as well
        node->next = NULL;
      }
      else
      {
        // This branch means this is the first node of the bucket so we remove the reference to it
        map->buckets[bucket_index] = NULL;
      }
      cct_delete_hash_map_node(node);
      return;
    }
    previous_node = node;
    node = node->next;
  }

  return;
}

ConcoctHashMapNode* get_first_node_in_bucket(const ConcoctHashMap* map, uint32_t bucket)
{
  return map->buckets[bucket];
}

uint32_t cct_get_hash_code(const char* str)
{
  uint32_t hash = CCT_HASH_OFFSET;
  char* first_byte = (char*)&hash;
  size_t str_length = strlen(str);

  for(size_t i = 0; i < str_length; i++)
  {
    *first_byte ^= str[i];
    hash *= CCT_HASH_PRIME;
  }

  return hash;
}
