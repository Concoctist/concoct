/*
 * Concoct - An imperative, dynamically-typed, interpreted, general-purpose programming language
 * Copyright (c) 2020-2022 BlakeTheBlock and Lloyd Dilley
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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h> // rand(), srand()
#include <time.h>   // time()
#include "debug.h"
#include "memory.h"
#include "types.h"

// Proof of concept to demonstrate garbage collection
size_t mark_objects()
{
  size_t mark_count = 0;
  if(debug_mode)
    debug_print("Marking objects...");
  for(size_t slot = 0; slot < get_store_capacity(); slot++)
  {
    if(object_store.objects[slot] != NULL && rand() % 2) // each object has a 50% chance of being marked
    {
      if(!object_store.objects[slot]->is_global && object_store.objects[slot]->const_name == NULL)
      {
        object_store.objects[slot]->is_garbage = true;
        mark_count++;
      }
    }
  }
  if(debug_mode)
    debug_print("%zu objects marked.", mark_count);
  return mark_count;
}

int main()
{
  debug_mode = true;
  uint8_t randnum = 0;
  srand((unsigned int)time(0));
  init_store();
  Object* objects[1024];
  for(size_t i = 0; i < 1024; i++)
  {
    printf("Object store free/capacity: %zu/%zu\n", get_store_free_slots(), get_store_capacity());
    printf("Used slots: %zu/%zu\n\n", get_store_used_slots(), get_store_capacity());
    print_store_total_size();
    randnum = rand() % 100 + 1;
    if(randnum <= 10)
      objects[i] = new_constant("100", "TEMP");
    else if(randnum > 10 && randnum <= 20)
      objects[i] = new_global("200");
    else
      objects[i] = new_object("7");
  }

  for(size_t i = 0; i < 1024; i++)
  {
    printf("Object of data type %s at slot #%zu is %zu bytes.\n", get_data_type(object_store.objects[i]), i, get_object_size(object_store.objects[i]));
  }
  mark_objects();
  collect_garbage(); // free only marked objects
  printf("Object store free/capacity: %zu/%zu\n", get_store_free_slots(), get_store_capacity());
  printf("Used slots: %zu/%zu\n\n", get_store_used_slots(), get_store_capacity());

  Object* object = new_object("null");
  printf("Data type: %s\n", get_data_type(object));
  print_object_value(object);
  printf("%s\n\n", (char *)get_object_value(object));

  object = new_object("true");
  printf("Data type: %s\n", get_data_type(object));
  print_object_value(object);
  printf("%d\n\n", *(Bool *)get_object_value(object));

  object = new_object("100");
  printf("Data type: %s\n", get_data_type(object));
  print_object_value(object);
  printf("%" PRId32 "\n\n", *(Number *)get_object_value(object));

  object = new_object("5721452096347253");
  printf("Data type: %s\n", get_data_type(object));
  print_object_value(object);
  printf("%" PRId64 "\n\n", *(BigNum *)get_object_value(object));

  object = new_object("77.715");
  printf("Data type: %s\n", get_data_type(object));
  print_object_value(object);
  printf("%f\n\n", *(Decimal *)get_object_value(object));

  object = new_object("Greetings, Concocter!");
  printf("Data type: %s\nString value: %s\nString length: %zu\n", get_data_type(object), object->value.strobj.strval, object->value.strobj.length);
  print_object_value(object);
  printf("Object size: %zu bytes\n", get_object_size(object));
  printf("%s\n", (char *)get_object_value(object));

  puts("\nAfter realloc_string():");
  realloc_string(&object->value.strobj, "Farewell, Concocter!");
  printf("Data type: %s\nString value: %s\nString length: %zu\n", get_data_type(object), object->value.strobj.strval, object->value.strobj.length);
  print_object_value(object);
  printf("%s\n", (char *)get_object_value(object));
  Object *object2 = clone_object(object);

  puts("\nCloned object:");
  printf("Data type: %s\nString value: %s\nString length: %zu\n", get_data_type(object2), object2->value.strobj.strval, object2->value.strobj.length);
  free_store();

  puts("\nStringify tests...");
  char* str = NULL;
  Number numval = 42;
  void* vptr = &numval;
  stringify(&str, vptr, NUMBER);
  printf("%s\n", str);
  free(str);
  Decimal decimalval = 57.05;
  vptr = &decimalval;
  stringify(&str, vptr, DECIMAL);
  printf("%s\n", str);
  free(str);

  char* testval = "Foo bar baz";
  vptr = &testval;
  stringify(&str, vptr, STRING);
  printf("%s\n", str);
  free(str);

  Bool boolval = false;
  vptr = &boolval;
  stringify(&str, vptr, BOOL);
  printf("%s\n", str);
  free(str);

  boolval = true;
  vptr = &boolval;
  stringify(&str, vptr, BOOL);
  printf("%s\n", str);
  free(str);

  char* nullval = NULL;
  vptr = &nullval;
  stringify(&str, vptr, NIL);
  printf("%s\n", str);
  free(str);

  return 0;
}
