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
#include <inttypes.h> // PRId32, PRId64
#include <math.h>     // round()
#include <stdio.h>    // fprintf(), stderr
#include <stdlib.h>   // calloc(), free(), malloc(), realloc(), EXIT_FAILURE
#include <string.h>   // memcpy(), snprintf(), strcpy(), strerror(), strlen()
#include "concoct.h"
#include "debug.h"
#include "memory.h"

ObjectStore object_store;

// Initializes object store
void init_store(void)
{
  // Ensure initial store capacity is >0 when using user input.
  object_store.objects = (Object **)calloc(INITIAL_STORE_CAPACITY, sizeof(Object *));
  if(object_store.objects == NULL)
  {
    fprintf(stderr, "Error allocating memory for object store: %s\n", strerror(errno));
    return;
  }
  object_store.capacity = INITIAL_STORE_CAPACITY;
  if(debug_mode)
    debug_print("Object store initialized with %zu slots.", INITIAL_STORE_CAPACITY);
  return;
}

// Reallocates memory for object store
void realloc_store(size_t new_size)
{
  Object** new_store = (Object **)realloc(object_store.objects, new_size * sizeof(Object *));
  if(new_store == NULL)
  {
    fprintf(stderr, "Error reallocating memory for object store: %s\n", strerror(errno));
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
void free_store(void)
{
  for(size_t slot = 0; slot < get_store_capacity(); slot++)
  {
    if(object_store.objects[slot] != NULL)
      free_object(&object_store.objects[slot]);
  }
  free(object_store.objects);
  if(debug_mode)
    debug_print("Object store freed.");
}

// Returns free slots of object store
size_t get_store_free_slots(void)
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
size_t get_store_used_slots(void)
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
size_t get_object_size(const Object* object)
{
  size_t obj_size = sizeof(Object);
  if(object->datatype == CCT_TYPE_STRING)
  {
    obj_size += sizeof(object->value.strobj);
    obj_size += object->value.strobj.length + 1; // +1 for null terminator
  }
  return obj_size;
}

// Returns total size of objects in object store in bytes
size_t get_store_objects_size(void)
{
  size_t total_size = 0;
  for(size_t slot = 0; slot < get_store_capacity(); slot++)
  {
    if(object_store.objects[slot] != NULL)
      total_size += get_object_size(object_store.objects[slot]);
  }
  return total_size;
}

// Prints total size of objects in object store
void print_store_objects_size(void)
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
void print_store_total_size(void)
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
  object->is_flagged = false;
  object->is_global = false;
  object->const_name = NULL;
  if(debug_mode)
    debug_print("Object of type %s created with value: %s", get_data_type(object), value);
  add_store_object(object);
  return object;
}

// Creates a new global object
Object* new_global(char* value)
{
  Object* object = (Object *)malloc(sizeof(Object));
  if(object == NULL)
  {
    fprintf(stderr, "Error allocating memory for global object: %s\n", strerror(errno));
    return NULL;
  }
  convert_type(object, value);
  object->is_flagged = false;
  object->is_global = true;
  object->const_name = NULL;
  if(debug_mode)
    debug_print("Global object of type %s created with value: %s", get_data_type(object), value);
  add_store_object(object);
  return object;
}

// Creates a new constant object
Object* new_constant(char* value, char* name)
{
  Object* object = (Object *)malloc(sizeof(Object));
  if(object == NULL)
  {
    fprintf(stderr, "Error allocating memory for constant object: %s\n", strerror(errno));
    return NULL;
  }
  convert_type(object, value);
  object->is_flagged = true;   // constants are never garbage collected
  object->is_global = false;
  object->const_name = name;
  if(debug_mode)
    debug_print("Constant object of type %s created with value: %s", get_data_type(object), value);
  add_store_object(object);
  return object;
}

// Populates Object struct based on datatype
Object* new_object_by_type(void* data, DataType datatype)
{
  Object* object = (Object *)malloc(sizeof(Object));
  if(object == NULL)
  {
    fprintf(stderr, "Error allocating memory for object: %s\n", strerror(errno));
    return NULL;
  }
  switch(datatype)
  {
    case CCT_TYPE_NIL:
      object->datatype = datatype;
      if(debug_mode)
        debug_print("Object of type %s created with value: null", get_type(datatype), stdout);
      break;
    case CCT_TYPE_STRING:
      object->datatype = datatype;
      new_string(&object->value.strobj, data);
      if(debug_mode)
        debug_print("Object of type %s created with value: %s", get_type(datatype), (char *)data, stdout);
      break;
    case CCT_TYPE_BOOL:
      object->datatype = datatype;
      object->value.boolval = *(Bool *)data;
      if(debug_mode)
        debug_print("Object of type %s created with value: %s", get_type(datatype), *(Bool *)data ? "true" : "false", stdout);
      break;
    case CCT_TYPE_BYTE:
      object->datatype = datatype;
      object->value.byteval = *(Byte *)data;
      if(debug_mode)
        debug_print("Object of type %s created with value: %u", get_type(datatype), *(Byte *)data);
      break;
    case CCT_TYPE_NUMBER:
      object->datatype = datatype;
      object->value.numval = *(Number *)data;
      if(debug_mode)
        debug_print("Object of type %s created with value: %" PRId32, get_type(datatype), *(Number *)data);
      break;
    case CCT_TYPE_BIGNUM:
      object->datatype = datatype;
      object->value.bignumval = *(BigNum *)data;
      if(debug_mode)
        debug_print("Object of type %s created with value: %" PRId64, get_type(datatype), *(BigNum *)data);
      break;
    case CCT_TYPE_DECIMAL:
      object->datatype = datatype;
      object->value.decimalval = *(Decimal *)data;
      if(debug_mode)
        debug_print("Object of type %s created with value: %f", get_type(datatype), *(Decimal *)data);
      break;
    default:
      fprintf(stderr, "Unsupported data type: %s\n", get_type(datatype));
      free(object);
      object = NULL;
      return NULL;
      break;
  }
  object->is_flagged = false;
  object->is_global = false;
  object->const_name = NULL;
  add_store_object(object);
  return object;
}

// Frees object
void free_object(Object** object)
{
  if((*object)->datatype == CCT_TYPE_STRING)
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

  if(object->datatype == CCT_TYPE_STRING)
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

// Converts numeric data to string
void stringify(char** str, void* data, DataType datatype)
{
  size_t length = 0;
  const char* nullstr = NULL;
  const char* boolstr = NULL;
  switch(datatype)
  {
    case CCT_TYPE_NIL:
      nullstr = "null";
      length = strlen(nullstr);
      *str = (char *)malloc(length + 1);
      if(*str == NULL)
      {
        fprintf(stderr, "Error allocating memory for null in stringify(): %s\n", strerror(errno));
        return;
      }
      strcpy(*str, nullstr);
      break;
    case CCT_TYPE_BOOL:
      boolstr = *(Bool *)data ? "true" : "false";
      length = strlen(boolstr);
      *str = (char *)malloc(length + 1);
      if(*str == NULL)
      {
        fprintf(stderr, "Error allocating memory for bool in stringify(): %s\n", strerror(errno));
        return;
      }
      strcpy(*str, boolstr);
      break;
    case CCT_TYPE_STRING:
      length = strlen(*(char **)data);
      *str = (char *)malloc(length + 1);
      if(*str == NULL)
      {
        fprintf(stderr, "Error allocating memory for string in stringify(): %s\n", strerror(errno));
        return;
      }
      strcpy(*str, *(char **)data);
      break;
    case CCT_TYPE_BYTE:
      length = snprintf(NULL, 0, "%u", *(Byte *)data);
      *str = (char *)malloc(length + 1);
      if(*str == NULL)
      {
        fprintf(stderr, "Error allocating memory for byte in stringify(): %s\n", strerror(errno));
        return;
      }
      snprintf(*str, length + 1, "%u", *(Byte *)data);
      break;
    case CCT_TYPE_NUMBER:
      length = snprintf(NULL, 0, "%" PRId32, *(Number *)data);
      *str = (char *)malloc(length + 1);
      if(*str == NULL)
      {
        fprintf(stderr, "Error allocating memory for number in stringify(): %s\n", strerror(errno));
        return;
      }
      snprintf(*str, length + 1, "%" PRId32, *(Number *)data);
      break;
    case CCT_TYPE_BIGNUM:
      length = snprintf(NULL, 0, "%" PRId64, *(BigNum *)data);
      *str = (char *)malloc(length + 1);
      if(*str == NULL)
      {
        fprintf(stderr, "Error allocating memory for big number in stringify(): %s\n", strerror(errno));
        return;
      }
      snprintf(*str, length + 1, "%" PRId64, *(BigNum *)data);
      break;
    case CCT_TYPE_DECIMAL:
      length = snprintf(NULL, 0, "%f", *(Decimal *)data);
      *str = (char *)malloc(length + 1);
      if(*str == NULL)
      {
        fprintf(stderr, "Error allocating memory for decimal in stringify(): %s\n", strerror(errno));
        return;
      }
      snprintf(*str, length + 1, "%f", *(Decimal *)data);
      break;
    default:
      fprintf(stderr, "Unrecognized data type in stringify()!\n");
      *str = NULL;
      break;
  }
  return;
}

// Flags objects to prevent them from being garbage collected and returns number of objects flagged
size_t flag_objects(Stack* stack)
{
  size_t flag_count = 0;
  if(debug_mode)
    debug_print("GC: Flagging objects...");
  for(size_t stack_slot = 0; stack_slot < stack->count; stack_slot++)
  {
    for(size_t store_slot = 0; store_slot < get_store_capacity(); store_slot++)
    {
      if(stack->objects[stack_slot] == object_store.objects[store_slot])
      {
        object_store.objects[store_slot]->is_flagged = true;
        flag_count++;
      }
    }
  }
  if(debug_mode)
    debug_print("GC: %zu objects flagged.", flag_count);
  return flag_count;
}

// Collects garbage and returns number of objects collected
size_t collect_garbage(void)
{
  size_t collect_count = 0;
  size_t old_store_size = get_store_objects_size();
  size_t size_difference = 0;

  if(debug_mode)
    debug_print("GC: Collecting garbage...");
  for(size_t slot = 0; slot < get_store_capacity(); slot++)
  {
    if(object_store.objects[slot] != NULL && !object_store.objects[slot]->is_flagged)
    {
      free_object(&object_store.objects[slot]);
      collect_count++;
    }
  }
  size_difference = old_store_size - get_store_objects_size();
  if(debug_mode)
  {
    if(size_difference < KILOBYTE_BOUNDARY)
      debug_print("GC: %zu objects collected. %zu bytes freed.\n", collect_count, size_difference);
    else if(size_difference > KILOBYTE_BOUNDARY && size_difference < MEGABYTE_BOUNDARY)
      debug_print("GC: %zu objects collected. %.3fKB freed.\n", collect_count, size_difference / 1024.0);
    else if(size_difference > MEGABYTE_BOUNDARY && size_difference < GIGABYTE_BOUNDARY)
      debug_print("GC: %zu objects collected. %.3fMB freed.\n", collect_count, size_difference / 1024.0 / 1024.0);
    else if(size_difference > GIGABYTE_BOUNDARY)
      debug_print("GC: %zu objects collected. %.3fGB freed.\n", collect_count, size_difference / 1024.0 / 1024.0 / 1024.0);
  }

  // Resize object store
  if(get_store_used_slots() != 0 && (size_t)round(get_store_capacity() / get_store_used_slots() * 100.0) >= STORE_SHRINK_THRESHOLD)
  {
    size_t new_size = (size_t)round(get_store_capacity() - (get_store_capacity() * (STORE_SHRINK_FACTOR / 100.0)));
    if(new_size >= INITIAL_STORE_CAPACITY)
      realloc_store(new_size);
  }

  // Reset object non-collection flag (constants are never collected)
  for(size_t slot = 0; slot < get_store_capacity(); slot++)
  {
    if(object_store.objects[slot] != NULL && object_store.objects[slot]->const_name == NULL)
      object_store.objects[slot]->is_flagged=false;
  }

  return collect_count;
}
