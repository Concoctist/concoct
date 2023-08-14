#include "hash_map.h"

ConcoctHashMap* cct_new_hash_map(int bucket_count)
{
  ConcoctHashMap* map = malloc(sizeof(ConcoctHashMap));
  map->bucket_count = bucket_count;
  map->buckets = malloc(bucket_count*sizeof(ConcoctHashMapNode*));
  for(int i = 0; i < bucket_count; i++)
  {
    map->buckets[i] = NULL;
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
  for(int i = 0; i < str_length; i++)
  {
    *first_byte ^= str[i];
    hash *= CCT_HASH_PRIME;
  }
  return hash;
}
