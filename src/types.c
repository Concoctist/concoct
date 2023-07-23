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
    case NIL:     return "null";
    case BOOL:    return "boolean";
    case BYTE:    return "byte";
    case NUMBER:  return "number";
    case BIGNUM:  return "big number";
    case DECIMAL: return "decimal";
    case STRING:  return "string";
    default:      return "unknown";
  }
}

// Returns string representation of data type from object
const char* get_data_type(Object* object)
{
  switch(object->datatype)
  {
    case NIL:     return "null";
    case BOOL:    return "boolean";
    case BYTE:    return "byte";
    case NUMBER:  return "number";
    case BIGNUM:  return "big number";
    case DECIMAL: return "decimal";
    case STRING:  return "string";
    default:      return "unknown";
  }
}

// Returns value of object
void* get_object_value(Object* object)
{
  switch(object->datatype)
  {
    case NIL:
      return NULL;
      break;
    case BOOL:
      return &object->value.boolval;
      break;
    case BYTE:
      return &object->value.byteval;
      break;
    case NUMBER:
      return &object->value.numval;
      break;
    case BIGNUM:
      return &object->value.bignumval;
      break;
    case DECIMAL:
      return &object->value.decimalval;
      break;
    case STRING:
      return object->value.strobj.strval;
      break;
  }
  return NULL;
}

// Displays value of object
void print_object_value(Object* object)
{
  switch(object->datatype)
  {
    case NIL:
      puts("null");
      break;
    case BOOL:
      if(object->value.boolval == false)
        puts("false");
      else
        puts("true");
      break;
    case BYTE:
      printf("%u\n", (Byte)object->value.byteval);
      break;
    case NUMBER:
      printf("%" PRId32 "\n", object->value.numval);
      break;
    case BIGNUM:
      printf("%" PRId64 "\n", object->value.bignumval);
      break;
    case DECIMAL:
      printf("%f\n", object->value.decimalval);
      break;
    case STRING:
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
    object->datatype = NIL;
    return;
  }

  // Handle booleans
#ifdef _WIN32
  if(_stricmp(value, "true") == 0)
#else
  if(strcasecmp(value, "true") == 0)
#endif
  {
    object->datatype = BOOL;
    object->value.boolval = true;
    return;
  }
#ifdef _WIN32
  if(_stricmp(value, "false") == 0)
#else
  if(strcasecmp(value, "false") == 0)
#endif
  {
    object->datatype = BOOL;
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
      object->datatype = NUMBER;
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
    object->datatype = BIGNUM;
    object->value.bignumval = bignum;
    return;
  }

  // Handle floats
  end = NULL;
  errno = 0;
  Decimal dec = strtod(value, &end);
  if(*end == '\0' && errno != ERANGE && errno != EINVAL)
  {
    object->datatype = DECIMAL;
    object->value.decimalval = dec;
    return;
  }

  // Default to string otherwise
  object->datatype = STRING;
  new_string(&object->value.strobj, value);
  return;
}
