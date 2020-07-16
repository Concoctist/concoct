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

#include <errno.h>  // errno
#include <math.h>   // round()
#include <stdio.h>  // fprintf(), stderr
#include <stdlib.h> // calloc(), free(), malloc(), realloc(), EXIT_FAILURE
#include <string.h> // memcpy(), strcpy(), strlen()
#include "debug.h"
#include "memory.h"

ObjectStore object_store;

// Initializes object store
void init_store()
{
	object_store.objects = (Object **)calloc(INITIAL_STORE_SIZE, sizeof(Object *));
	if(object_store.objects == NULL)
	{
		fprintf(stderr, "Error allocating memory for object store: %s\n", strerror(errno));
		return;
	}
	object_store.capacity = INITIAL_STORE_SIZE;
	if(debug_mode)
		debug_print("Object store initialized with %zu slots.", INITIAL_STORE_SIZE);
	return;
}

// Reallocates memory for object store
void realloc_store(size_t new_size)
{
	Object** new_store = (Object **)realloc(object_store.objects, new_size * sizeof(Object *));
	if(new_store == NULL)
	{
		fprintf(stderr, "Error reallocating memory for object store: %s\n", strerror(errno));
		//exit(EXIT_FAILURE);
		return;
	}
	// Initialize new space
	for(size_t slot = get_store_capacity(); slot < new_size; slot++)
		new_store[slot] = NULL;
	object_store.objects = new_store;
	if(debug_mode)
		debug_print("Object store resized from %zu to %zu slots.", object_store.capacity, new_size);
	object_store.capacity = new_size;
	return;
}

// Frees object store
void free_store()
{
	free(object_store.objects);
	if(debug_mode)
		debug_print("Object store freed.");
}

// Returns free slots of object store
size_t get_store_free_slots()
{
	size_t free_slots = 0;
	for(size_t slot = 0; slot < get_store_capacity(); slot++)
	{
		if(object_store.objects[slot] == NULL)
			free_slots += 1;
	}
	return free_slots;
}

// Returns used slots of object store
size_t get_store_used_slots()
{
	size_t used_slots = 0;
	for(size_t slot = 0; slot < get_store_capacity(); slot++)
	{
		if(object_store.objects[slot] != NULL)
		used_slots += 1;
	}
	return used_slots;
}

// Returns size of object in bytes
size_t get_object_size(Object* object)
{
	size_t obj_size = sizeof(Object);
	if(object->datatype == STRING)
	{
		obj_size += sizeof(object->value.strobj);
		obj_size += object->value.strobj.length + 1; // +1 for null terminator
	}
	return obj_size;
}

// Returns total size of objects in object store in bytes
size_t get_store_objects_size()
{
	size_t total_size = 0;
	for(size_t slot = 0; slot < get_store_capacity(); slot++)
	{
		if(object_store.objects[slot] != NULL)
			total_size += get_object_size(object_store.objects[slot]);
	}
	return total_size;
}

// Returns total size of object store in bytes
size_t get_store_total_size()
{
	return get_store_objects_size() + sizeof(ObjectStore) + sizeof(Object *) * get_store_capacity();
}

// Prints total size of objects in object store
void print_store_objects_size()
{
	size_t total_size = get_store_objects_size();
	printf("Total objects size in store: ");
	if(total_size < KILOBYTE_BOUNDARY)
		printf("%zu bytes\n", total_size);
	else if(total_size > KILOBYTE_BOUNDARY && total_size < MEGABYTE_BOUNDARY)
		printf("%.3fKB\n", total_size / 1024.0);
	else if(total_size > MEGABYTE_BOUNDARY && total_size < GIGABYTE_BOUNDARY)
		printf("%.3fMB\n", total_size / 1024.0 / 1024.0);
	else if(total_size > GIGABYTE_BOUNDARY)
		printf("%.3fGB\n", total_size / 1024.0 / 1024.0 / 1024.0);
	return;
}

// Prints total size of object store
void print_store_total_size()
{
	size_t total_size = get_store_total_size();
	printf("Object store total size: ");
	if(total_size < KILOBYTE_BOUNDARY)
		printf("%zu bytes\n", total_size);
	else if(total_size > KILOBYTE_BOUNDARY && total_size < MEGABYTE_BOUNDARY)
		printf("%.3fKB\n", total_size / 1024.0);
	else if(total_size > MEGABYTE_BOUNDARY && total_size < GIGABYTE_BOUNDARY)
		printf("%.3fMB\n", total_size / 1024.0 / 1024.0);
	else if(total_size > GIGABYTE_BOUNDARY)
		printf("%.3fGB\n", total_size / 1024.0 / 1024.0 / 1024.0);
	return;
}

// Converts bytes to kilobytes
size_t convert_kilobytes(size_t bytes)
{
	return (size_t)round(bytes / 1024.0);
}

// Converts bytes to megabytes
size_t convert_megabytes(size_t bytes)
{
	return (size_t)round(convert_kilobytes(bytes) / 1024.0);
}

// Converts bytes to gigabytes
size_t convert_gigabytes(size_t bytes)
{
	return (size_t)round(convert_megabytes(bytes) / 1024.0);
}

// Adds object to store
void add_store_object(Object* object)
{
	for(size_t slot = 0; slot < get_store_capacity(); slot++)
	{
		if(slot >= (size_t)round(get_store_capacity() - ((STORE_GROWTH_THRESHOLD / 100.0) * get_store_capacity())))
			realloc_store((size_t)round(get_store_capacity() + (get_store_capacity() * (STORE_GROWTH_FACTOR / 100.0))));
		if(object_store.objects[slot] == NULL)
		{
			object_store.objects[slot] = object;
			if(debug_mode)
				debug_print("Object of type %s added to object store at slot %zu.", get_data_type(object), slot);
			return;
		}
	}
	return;
}

// Populates String struct
void new_string(String* strobj, char* str)
{
	strobj->strval = (char *)malloc(strlen(str) + 1);
	if(strobj->strval == NULL)
	{
		fprintf(stderr, "Error allocating memory for string (\"%s\"): %s\n", str, strerror(errno));
		return;
	}
	strcpy(strobj->strval, str);
	strobj->length = strlen(str);
	if(debug_mode)
		debug_print("Memory allocated for string with length of %zu characters: %s", strobj->length, str);
	return;
}

// Reallocates memory for string
void realloc_string(String* strobj, const char* new_string)
{
	if(debug_mode)
		debug_print("Reallocation attempt for original string containing %zu characters: %s", strobj->length, strobj->strval);
	char* newstr = (char *)realloc(strobj->strval, strlen(new_string) + 1);
	if(newstr == NULL)
	{
		fprintf(stderr, "Error reallocating memory for string (\"%s\"): %s\n", new_string, strerror(errno));
		return;
	}
	strcpy(newstr, new_string);
	if(debug_mode)
		debug_print("Memory successfully reallocated for string with length of %zu characters: %s", strlen(new_string), new_string);
	strobj->strval = newstr;
	strobj->length = strlen(new_string);
	return;
}

// Frees string
void free_string(String* strobj)
{
	free(strobj->strval);
	if(debug_mode)
		debug_print("String freed.");
	strobj->length = 0;
	return;
}

// Populates Object struct
Object* new_object(char* value)
{
	Object* object = (Object *)malloc(sizeof(Object));
	if(object == NULL)
	{
		fprintf(stderr, "Error allocating memory for object: %s\n", strerror(errno));
		return NULL;
	}
	convert_type(object, value);
	object->marked = false;
	if(debug_mode)
		debug_print("Object of type %s created with value: %s", get_data_type(object), value);
	add_store_object(object);
	return object;
}

// Frees object
void free_object(Object** object)
{
	if((*object)->datatype == STRING)
		free_string(&(*object)->value.strobj);
	free(*object);
	*object = NULL;
	if(debug_mode)
		debug_print("Object freed.");
	return;
}

// Clones object
Object* clone_object(Object* object)
{
	Object* new_object = (Object *)malloc(sizeof(Object));
	if(new_object == NULL)
	{
		fprintf(stderr, "Error allocating memory for object during cloning: %s\n", strerror(errno));
		return NULL;
	}
	memcpy(new_object, object, sizeof(Object));

	if(object->datatype == STRING)
	{
		new_string(&new_object->value.strobj, object->value.strobj.strval);
		if(new_object->value.strobj.strval == NULL)
		{
			free(new_object);
			return NULL;
		}
	}
	if(debug_mode)
		debug_print("Object of type %s cloned.", get_data_type(object));
	add_store_object(new_object);

	return new_object;
}
