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

#ifndef CCT_HASH_MAP_H
#define CCT_HASH_MAP_H

#include <stdbool.h> // false, true
#include <stdint.h>  // uint32_t

#define CCT_HASH_OFFSET ((uint32_t)2166136261)
#define CCT_HASH_PRIME ((uint32_t)16777619)

typedef struct ConcoctHashMapNode ConcoctHashMapNode;

struct ConcoctHashMapNode
{
  uint32_t hash;
  const char* key;
  void* value;
  ConcoctHashMapNode* next;
};

typedef struct concoct_hash_map
{
  ConcoctHashMapNode** buckets;
  uint32_t bucket_count;
} ConcoctHashMap;

ConcoctHashMapNode* cct_new_hash_map_node(const char* key, void* value, uint32_t hash);
void cct_delete_hash_map_node(ConcoctHashMapNode* map);

ConcoctHashMap* cct_new_hash_map(uint32_t bucket_count);
void cct_delete_hash_map(ConcoctHashMap* map);

bool cct_hash_map_has_key(const ConcoctHashMap* map, const char* key);
void cct_hash_map_set(ConcoctHashMap* map, const char* key, void* value);
void* cct_hash_map_get(const ConcoctHashMap* map, const char* key);
void cct_hash_map_delete_entry(ConcoctHashMap* map, const char* key);

ConcoctHashMapNode* get_first_node_in_bucket(const ConcoctHashMap* map, uint32_t bucket);

uint32_t cct_get_hash_code(const char* str);

#endif // CCT_HASH_MAP_H
