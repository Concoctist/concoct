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

#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"
#include "stack.h"

// Initial free store capacity
static const size_t INITIAL_STORE_CAPACITY = 128;
// Percentage to increase object store by during expansion
static const uint8_t STORE_GROWTH_FACTOR = 50;
// Percentage of free slots remaining in object store before triggering expansion
static const uint8_t STORE_GROWTH_THRESHOLD = 10;
// Percentage to decrease object store by during compaction
static const uint8_t STORE_SHRINK_FACTOR = 25;
// Percentage of free slots remaining in object store before triggering compaction
static const uint8_t STORE_SHRINK_THRESHOLD = 75;

// Byte size limits for conversions
static const size_t KILOBYTE_BOUNDARY = 1024;
static const size_t MEGABYTE_BOUNDARY = 1048576;
static const size_t GIGABYTE_BOUNDARY = 1073741824;

// Object store
typedef struct objstore
{
  size_t capacity;
  Object** objects;
} ObjectStore;
extern ObjectStore object_store;

// Initializes object store
void init_store(void);

// Reallocates memory for object store
void realloc_store(size_t new_size);

// Frees object store
void free_store(void);

// Returns size of object store
static inline size_t get_store_capacity(void) { return object_store.capacity; }

// Returns free slots of object store
size_t get_store_free_slots(void);

// Returns used slots of object store
size_t get_store_used_slots(void);

// Returns size of object in bytes
size_t get_object_size(const Object* object);

// Returns total size of objects in object store in bytes
size_t get_store_objects_size(void);

// Returns total size of object store in bytes
static inline size_t get_store_total_size(void) { return get_store_objects_size() + sizeof(ObjectStore) + sizeof(Object *) * get_store_capacity(); }

// Prints total size of objects in object store
void print_store_objects_size(void);

// Prints total size of object store
void print_store_total_size(void);

// Converts bytes to kilobytes
size_t convert_kilobytes(size_t bytes);

// Converts bytes to megabytes
size_t convert_megabytes(size_t bytes);

// Converts bytes to gigabytes
size_t convert_gigabytes(size_t bytes);

// Adds object to store
void add_store_object(Object* object);

// Populates String struct
void new_string(String* strobj, char* str);

// Reallocates memory for string
void realloc_string(String* strobj, const char* new_string);

// Frees string
void free_string(String* strobj);

// Populates Object struct
Object* new_object(char* value);

// Creates a new global object
Object* new_global(char* value);

// Creates a new constant object
Object* new_constant(char* value, char* name);

// Populates Object struct based on datatype
Object* new_object_by_type(void* data, DataType datatype);

// Frees object
void free_object(Object** object);

// Clones object
Object* clone_object(Object* object);

// Converts numeric data to string
void stringify(char** str, void* data, DataType datatype);

// Flags objects to prevent them from being garbage collected and returns number of objects flagged
size_t flag_objects(Stack* stack);

// Collects garbage and returns number of objects collected
size_t collect_garbage(void);

#endif // MEMORY_H
