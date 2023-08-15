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

#include "hash_map.h"

ConcoctHashMap* cct_new_hash_map(int bucket_count)
{
  ConcoctHashMap* map = malloc(sizeof(ConcoctHashMap));
  if(map == NULL)
  {
    printf("Failed to allocate memory for a hash map.");
    abort();
  }

  map->bucket_count = bucket_count;
  map->buckets = malloc(bucket_count * sizeof(ConcoctHashMapNode*));
  if(map->buckets == NULL)
  {
    printf("Failed to allocate memory for a hash map buckets.");
    abort();
  }

  for(int i = 0; i < bucket_count; i++)
  {
    ConcoctHashMapNode** bucket = map->buckets + i;
    *bucket = NULL;
  }
  return map;
}

void cct_delete_hash_map(ConcoctHashMap* map)
{
  for(int i = 0; i < map->bucket_count; i++)
  {
    if(map->buckets[i] != NULL)
    {
      cct_delete_hash_map_node(map->buckets[i]);
    }
  }
  free(map->buckets);
  free(map);
}

ConcoctHashMapNode* cct_new_hash_map_node(const char* key, void* value, unsigned int hash)
{
  ConcoctHashMapNode* node = malloc(sizeof(ConcoctHashMapNode));
  if(node == NULL)
  {
    printf("Failed to allocate memory for a hash map key.");
    abort();
  }

  node->hash = hash;
  node->key = key;
  node->value = value;
  node->next = NULL;
  return node;
}

void cct_delete_hash_map_node(ConcoctHashMapNode* node)
{
  if(node->next != NULL)
  {
    cct_delete_hash_map_node(node->next);
  }
  free(node);
}

int cct_hash_map_has_key(ConcoctHashMap* map, const char* key)
{
  // Finds the bucket that this key could be in and checks each entry in it
  unsigned int hash = cct_get_hash_code(key);
  int bucket_index = hash % map->bucket_count;
  ConcoctHashMapNode* node = map->buckets[bucket_index];
  while(node != NULL)
  {
    // Checks hash first for efficiency. Checks the string itself in case of hash collisions
    if(node->hash == hash && strcmp(node->key, key) == 0)
    {
      return 1;
    }
    node = node->next;
  }
  return 0;
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
}

void* cct_hash_map_get(ConcoctHashMap* map, const char* key)
{
  // Finds the bucket that this key could be in and checks each entry in it
  unsigned int hash = cct_get_hash_code(key);
  int bucket_index = hash % map->bucket_count;
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
  unsigned int hash = cct_get_hash_code(key);
  int bucket_index = hash % map->bucket_count;
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
}

ConcoctHashMapNode* get_first_node_in_bucket(ConcoctHashMap* map, int bucket)
{
  return map->buckets[bucket];
}
unsigned int cct_get_hash_code(const char* str)
{
  unsigned int hash = CCT_HASH_OFFSET;
  char* first_byte = (char*)&hash;

  size_t str_length = strlen(str);
  for(size_t i = 0; i < str_length; i++)
  {
    *first_byte ^= str[i];
    hash *= CCT_HASH_PRIME;
  }
  return hash;
}
