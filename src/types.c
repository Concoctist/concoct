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

#include <errno.h>    // errno, EINVAL, ERANGE
#include <inttypes.h> // PRId32, PRId64
#include <limits.h>   // INT32_MIN, INT32_MAX
#include <stdio.h>    // printf(), puts()
#include <stdlib.h>   // strtod(), strtol(), strtoll()
#include <string.h>   // strcasecmp()
#include "memory.h"
#include "types.h"

// Returns string representation of data type
const char* get_type(DataType datatype)
{
  switch(datatype)
  {
    case CCT_TYPE_NIL:     return "null";
    case CCT_TYPE_BOOL:    return "boolean";
    case CCT_TYPE_BYTE:    return "byte";
    case CCT_TYPE_NUMBER:  return "number";
    case CCT_TYPE_BIGNUM:  return "big number";
    case CCT_TYPE_DECIMAL: return "decimal";
    case CCT_TYPE_STRING:  return "string";
    default:               return "unknown";
  }
}

// Returns string representation of data type from object
const char* get_data_type(const Object* object)
{
  switch(object->datatype)
  {
    case CCT_TYPE_NIL:     return "null";
    case CCT_TYPE_BOOL:    return "boolean";
    case CCT_TYPE_BYTE:    return "byte";
    case CCT_TYPE_NUMBER:  return "number";
    case CCT_TYPE_BIGNUM:  return "big number";
    case CCT_TYPE_DECIMAL: return "decimal";
    case CCT_TYPE_STRING:  return "string";
    default:               return "unknown";
  }
}

// Returns value of object
void* get_object_value(Object* object)
{
  switch(object->datatype)
  {
    case CCT_TYPE_NIL:
      return NULL;
    case CCT_TYPE_BOOL:
      return &object->value.boolval;
    case CCT_TYPE_BYTE:
      return &object->value.byteval;
    case CCT_TYPE_NUMBER:
      return &object->value.numval;
    case CCT_TYPE_BIGNUM:
      return &object->value.bignumval;
    case CCT_TYPE_DECIMAL:
      return &object->value.decimalval;
    case CCT_TYPE_STRING:
      return object->value.strobj.strval;
  }
  return NULL;
}

// Displays value of object
void print_object_value(Object* object)
{
  switch(object->datatype)
  {
    case CCT_TYPE_NIL:
      puts("null");
      break;
    case CCT_TYPE_BOOL:
      if(object->value.boolval == false)
        puts("false");
      else
        puts("true");
      break;
    case CCT_TYPE_BYTE:
      printf("%u\n", object->value.byteval);
      break;
    case CCT_TYPE_NUMBER:
      printf("%" PRId32 "\n", object->value.numval);
      break;
    case CCT_TYPE_BIGNUM:
      printf("%" PRId64 "\n", object->value.bignumval);
      break;
    case CCT_TYPE_DECIMAL:
      printf("%f\n", object->value.decimalval);
      break;
    case CCT_TYPE_STRING:
      printf("%s\n", object->value.strobj.strval);
      break;
  }
}

// Converts string to applicable type
void convert_type(Object* object, char* value)
{
  // Handle null
#ifdef _WIN32
  if(_stricmp(value, "null") == 0)
#else
  if(strcasecmp(value, "null") == 0)
#endif
  {
    object->datatype = CCT_TYPE_NIL;
    return;
  }

  // Handle booleans
#ifdef _WIN32
  if(_stricmp(value, "true") == 0)
#else
  if(strcasecmp(value, "true") == 0)
#endif
  {
    object->datatype = CCT_TYPE_BOOL;
    object->value.boolval = true;
    return;
  }
#ifdef _WIN32
  if(_stricmp(value, "false") == 0)
#else
  if(strcasecmp(value, "false") == 0)
#endif
  {
    object->datatype = CCT_TYPE_BOOL;
    object->value.boolval = false;
    return;
  }

  // Handle integers
  char *end = NULL;
  errno = 0;
  BigNum bignum = strtol(value, &end, 10);
  if(*end == '\0' && errno != ERANGE && errno != EINVAL)
  {
    if(bignum > INT32_MIN && bignum < INT32_MAX)
    {
      object->datatype = CCT_TYPE_NUMBER;
      object->value.numval = (Number)bignum;
      return;
    }
  }

  // Handle big integers
  end = NULL;
  errno = 0;
  bignum = strtoll(value, &end, 10);
  if(*end == '\0' && errno != ERANGE && errno != EINVAL)
  {
    object->datatype = CCT_TYPE_BIGNUM;
    object->value.bignumval = bignum;
    return;
  }

  // Handle floats
  end = NULL;
  errno = 0;
  Decimal dec = strtod(value, &end);
  if(*end == '\0' && errno != ERANGE && errno != EINVAL)
  {
    object->datatype = CCT_TYPE_DECIMAL;
    object->value.decimalval = dec;
    return;
  }

  // Default to string otherwise
  object->datatype = CCT_TYPE_STRING;
  new_string(&object->value.strobj, value);
  return;
}
