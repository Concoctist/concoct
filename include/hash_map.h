#ifndef CCT_HASH_MAP_H
#define CCT_HASH_MAP_H

#include <stdlib.h>
#include <string.h>

#define CCT_HASH_OFFSET 2166136261
#define CCT_HASH_PRIME 16777619

typedef struct ConcoctHashMapNode ConcoctHashMapNode;

struct ConcoctHashMapNode {
  unsigned int hash;
  const char* key;
  void* value;
  ConcoctHashMapNode* next;
};

typedef struct concoct_hash_map {
  ConcoctHashMapNode** buckets;
  int bucket_count;
} ConcoctHashMap;

ConcoctHashMapNode* cct_new_hash_map_node(const char* key, void* value, unsigned int hash);
void cct_delete_hash_map_node(ConcoctHashMapNode* map);

ConcoctHashMap* cct_new_hash_map(int bucket_count);
void cct_delete_hash_map(ConcoctHashMap* map);

int cct_hash_map_has_key(ConcoctHashMap* map, const char* key);
void cct_hash_map_set(ConcoctHashMap* map, const char* key, void* value);
void* cct_hash_map_get(ConcoctHashMap* map, const char* key);
void cct_hash_map_delete_entry(ConcoctHashMap* map, const char* key);

ConcoctHashMapNode* get_first_node_in_bucket(ConcoctHashMap* map, int bucket);

unsigned int cct_get_hash_code(const char* str);

#endif
