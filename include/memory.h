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

static const size_t INITIAL_STORE_SIZE = 128;
static const float STORE_GROWTH_FACTOR = 0.5f;
static const size_t STORE_GROWTH_THRESHOLD = 8;

// Object store
typedef struct objstore
{
	size_t length;
	Object** objects;
} ObjectStore;
ObjectStore object_store;

// Initialize object store
void init_store();

// Reallocate memory for object store
void realloc_store(size_t new_size);

// Free object store
void free_store();

// Return size of object store
static inline size_t get_store_size() { return object_store.length; }

// Return free slots of object store
size_t get_store_free_slots();

// Add object to store
void add_store_object(Object* object);

// Populate String struct
void new_string(String* strobj, char* str);

// Reallocate memory for string
void realloc_string(String* strobj, const char* new_string);

// Free string
void free_string(String* strobj);

// Populate Object struct
Object* new_object(char* value);

// Free object
void free_object(Object* object);

// Clone object
Object* clone_object(Object* object);

#endif // MEMORY_H
