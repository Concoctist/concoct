/*
 * Concoct - An imperative, dynamically-typed, interpreted, general-purpose programming language
 * Copyright (c) 2020 BlakeTheBlock and Lloyd Dilley
 * http://concoct.dev/
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

// Initial free store capacity
static const size_t INITIAL_STORE_SIZE = 128;
// Percentage to increase object store by during expansion
static const size_t STORE_GROWTH_FACTOR = 50;
// Percentage of free slots remaining in object store before triggering an expansion
static const size_t STORE_GROWTH_THRESHOLD = 10;

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
void init_store();

// Reallocates memory for object store
void realloc_store(size_t new_size);

// Frees object store
void free_store();

// Returns size of object store
static inline size_t get_store_capacity() { return object_store.capacity; }

// Returns free slots of object store
size_t get_store_free_slots();

// Returns used slots of object store
size_t get_store_used_slots();

// Returns size of object in bytes
size_t get_object_size(Object* object);

// Returns total size of objects in object store in bytes
size_t get_store_objects_size();

// Returns total size of object store in bytes
size_t get_store_total_size();

// Prints total size of objects in object store
void print_store_objects_size();

// Prints total size of object store
void print_store_total_size();

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

// Frees object
void free_object(Object** object);

// Clones object
Object* clone_object(Object* object);

#endif // MEMORY_H
