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

#include <stdio.h>
#include "debug.h"
#include "memory.h"
#include "stack.h"
#include "types.h"
#include "vm/instructions.h"

int main(void)
{
  debug_mode = true;
  Stack stack;
  Stack* pstack = &stack;
  init_stack(pstack);
  init_store();

  Object* object = new_object("null");
  push(pstack, object);
  printf("Data type: %s\n", get_data_type(peek(pstack)));
  print_object_value(peek(pstack));

  object = new_object("true");
  push(pstack, object);
  printf("Data type: %s\n", get_data_type(peek(pstack)));
  print_object_value(peek(pstack));

  object = new_object("100");
  push(pstack, object);
  printf("Data type: %s\n", get_data_type(peek(pstack)));
  print_object_value(peek(pstack));

  object = new_object("5721452096347253");
  push(pstack, object);
  printf("Data type: %s\n", get_data_type(peek(pstack)));
  print_object_value(peek(pstack));

  object = new_object("77.715");
  push(pstack, object);
  printf("Data type: %s\n", get_data_type(peek(pstack)));
  print_object_value(peek(pstack));

  object = new_object("Greetings, Concocter!");
  push(pstack, object);
  printf("Data type: %s\n", get_data_type(peek(pstack)));
  print_object_value(peek(pstack));

  puts("\nValue of each stack item after pop():");
  for(size_t i = pstack->count; i > 0; i--)
    print_object_value(pop(pstack));

  puts("\nAdding false and false to stack...");
  object = new_object("false");
  push(pstack, object);
  object = new_object("false");
  push(pstack, object);
  puts("Anding result...");
  op_and(pstack);
  puts("Value after anding stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding true and false to stack...");
  object = new_object("true");
  push(pstack, object);
  object = new_object("false");
  push(pstack, object);
  puts("Anding result...");
  op_and(pstack);
  puts("Value after anding stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding true and true to stack...");
  object = new_object("true");
  push(pstack, object);
  object = new_object("true");
  push(pstack, object);
  puts("Anding result...");
  op_and(pstack);
  puts("Value after anding stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding false to stack...");
  object = new_object("false");
  push(pstack, object);
  puts("Negating result...");
  op_not(pstack);
  puts("Value after negating stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding true to stack...");
  object = new_object("true");
  push(pstack, object);
  puts("Negating result...");
  op_not(pstack);
  puts("Value after negating stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding false and false to stack...");
  object = new_object("false");
  push(pstack, object);
  object = new_object("false");
  push(pstack, object);
  puts("Oring result...");
  op_or(pstack);
  puts("Value after oring stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding true and false to stack...");
  object = new_object("true");
  push(pstack, object);
  object = new_object("false");
  push(pstack, object);
  puts("Oring result...");
  op_or(pstack);
  puts("Value after oring stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding true and true to stack...");
  object = new_object("true");
  push(pstack, object);
  object = new_object("true");
  push(pstack, object);
  puts("Oring result...");
  op_or(pstack);
  puts("Value after oring stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding 5 and 5.0 to stack...");
  object = new_object("5");
  push(pstack, object);
  object = new_object("5.0");
  push(pstack, object);
  puts("Checking equality of result...");
  op_eql(pstack);
  puts("Value after checking equality of stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding 5 and 5.0 to stack...");
  object = new_object("5");
  push(pstack, object);
  object = new_object("5.0");
  push(pstack, object);
  puts("Checking inequality of result...");
  op_neq(pstack);
  puts("Value after checking inequality of stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding 5 and 5.5 to stack...");
  object = new_object("5");
  push(pstack, object);
  object = new_object("5.5");
  push(pstack, object);
  puts("Checking equality of result...");
  op_eql(pstack);
  puts("Value after checking equality of stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding 5 and 5.5 to stack...");
  object = new_object("5");
  push(pstack, object);
  object = new_object("5.5");
  push(pstack, object);
  puts("Checking inequality of result...");
  op_neq(pstack);
  puts("Value after checking inequality of stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding \"foo\" and \"foo\" to stack...");
  object = new_object("foo");
  push(pstack, object);
  object = new_object("foo");
  push(pstack, object);
  puts("Checking equality of result...");
  op_eql(pstack);
  puts("Value after checking equality of stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding \"foo\" and \"foo\" to stack...");
  object = new_object("foo");
  push(pstack, object);
  object = new_object("foo");
  push(pstack, object);
  puts("Checking inequality of result...");
  op_neq(pstack);
  puts("Value after checking inequality of stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding \"foo\" and \"bar\" to stack...");
  object = new_object("foo");
  push(pstack, object);
  object = new_object("bar");
  push(pstack, object);
  puts("Checking equality of result...");
  op_eql(pstack);
  puts("Value after checking equality of stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding \"foo\" and \"bar\" to stack...");
  object = new_object("foo");
  push(pstack, object);
  object = new_object("bar");
  push(pstack, object);
  puts("Checking inequality of result...");
  op_neq(pstack);
  puts("Value after checking inequality of stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding null and null to stack...");
  object = new_object("null");
  push(pstack, object);
  object = new_object("null");
  push(pstack, object);
  puts("Checking equality of result...");
  op_eql(pstack);
  puts("Value after checking equality of stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding null and null to stack...");
  object = new_object("null");
  push(pstack, object);
  object = new_object("null");
  push(pstack, object);
  puts("Checking inequality of result...");
  op_neq(pstack);
  puts("Value after checking inequality of stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding 3 and 10 to stack...");
  object = new_object("3");
  push(pstack, object);
  object = new_object("10");
  push(pstack, object);
  puts("Checking >= of result...");
  op_gte(pstack);
  puts("Value after checking >= of stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding 3 and 10 to stack...");
  object = new_object("3");
  push(pstack, object);
  object = new_object("10");
  push(pstack, object);
  puts("Checking <= of result...");
  op_lte(pstack);
  puts("Value after checking <= of stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding 3 and 3 to stack...");
  object = new_object("3");
  push(pstack, object);
  object = new_object("3");
  push(pstack, object);
  puts("Checking <= of result...");
  op_lte(pstack);
  puts("Value after checking <= of stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding 20 to stack...");
  object = new_object("20");
  push(pstack, object);
  puts("Decrementing stack value...");
  op_dec(pstack);
  puts("Value after decrementing stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding 20 to stack...");
  object = new_object("20");
  push(pstack, object);
  puts("Incrementing stack value...");
  op_inc(pstack);
  puts("Value after incrementing stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding 3 to stack...");
  object = new_object("3");
  push(pstack, object);
  puts("Adding 7 to stack...");
  object = new_object("7");
  push(pstack, object);
  puts("Adding stack values...");
  op_add(pstack);
  puts("Value after adding stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding 10 to stack...");
  object = new_object("10");
  push(pstack, object);
  puts("Adding 3 to stack...");
  object = new_object("3");
  push(pstack, object);
  puts("Subtracting stack values...");
  op_sub(pstack);
  puts("Value after subtracting stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding 10 to stack...");
  object = new_object("10");
  push(pstack, object);
  puts("Adding 5 to stack...");
  object = new_object("5");
  push(pstack, object);
  puts("Dividing stack values...");
  op_div(pstack);
  puts("Value after dividing stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding 10 to stack...");
  object = new_object("10");
  push(pstack, object);
  puts("Adding 0 to stack...");
  object = new_object("0");
  push(pstack, object);
  puts("Dividing stack values (zero test)...");
  if(op_div(pstack) == RUN_ERROR)
    puts("Runtime error encountered!");
  else
  {
    puts("Value after dividing stack contents:");
    print_object_value(pop(pstack));
  }

  puts("\nAdding 5 to stack...");
  object = new_object("5");
  push(pstack, object);
  puts("Adding 2 to stack...");
  object = new_object("2");
  push(pstack, object);
  puts("Multiplying stack values...");
  op_mul(pstack);
  puts("Value after multiplying stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding 11 to stack...");
  object = new_object("11");
  push(pstack, object);
  puts("Adding 5 to stack...");
  object = new_object("5");
  push(pstack, object);
  puts("Modulating stack values...");
  op_mod(pstack);
  puts("Value after modulating stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding 5 to stack...");
  object = new_object("5");
  push(pstack, object);
  puts("Adding 2 to stack...");
  object = new_object("2");
  push(pstack, object);
  puts("Exponentiating stack values...");
  op_pow(pstack);
  puts("Value after exponentiating stack contents:");
  print_object_value(pop(pstack));

  puts("\nAdding 32 to stack...");
  object = new_object("32");
  push(pstack, object);
  puts("Adding 32 to stack...");
  object = new_object("32");
  push(pstack, object);
  puts("Performing bitwise and...");
  op_bnd(pstack);
  puts("Value after performing bitwise and:");
  print_object_value(pop(pstack));

  puts("\nAdding 32 to stack...");
  object = new_object("32");
  push(pstack, object);
  puts("Adding 32 to stack...");
  object = new_object("32");
  push(pstack, object);
  puts("Performing bitwise or...");
  op_bor(pstack);
  puts("Value after performing bitwise or:");
  print_object_value(pop(pstack));

  puts("\nAdding 32 to stack...");
  object = new_object("32");
  push(pstack, object);
  puts("Adding 32 to stack...");
  object = new_object("32");
  push(pstack, object);
  puts("Performing bitwise xor...");
  op_xor(pstack);
  puts("Value after performing bitwise xor:");
  print_object_value(pop(pstack));

  puts("\nAdding 32 to stack...");
  object = new_object("32");
  push(pstack, object);
  puts("Performing bitwise not...");
  op_bnt(pstack);
  puts("Value after performing bitwise not:");
  print_object_value(pop(pstack));

  puts("\nAdding 1 to stack...");
  object = new_object("1");
  push(pstack, object);
  puts("Adding 8 to stack...");
  object = new_object("8");
  push(pstack, object);
  puts("Bit shifting stack value to the left...");
  op_shl(pstack);
  puts("Value after bit shifting stack contents to the left:");
  print_object_value(pop(pstack));

  puts("\nAdding 256 to stack...");
  object = new_object("256");
  push(pstack, object);
  puts("Adding 4 to stack...");
  object = new_object("4");
  push(pstack, object);
  puts("Bit shifting stack value to the right...");
  op_shr(pstack);
  puts("Value after bit shifting stack contents to the right:");
  print_object_value(pop(pstack));

  object = new_object("false");
  push(pstack, object);
  puts("\nValue after adding \"false\" to stack:");
  print_object_value(pop(pstack));

  object = new_object("true");
  push(pstack, object);
  puts("\nValue after adding \"true\" to stack:");
  print_object_value(pop(pstack));

  puts("\nAdding 327.98 to stack...");
  object = new_object("327.98");
  push(pstack, object);
  op_neg(pstack);
  puts("Value after calling op_neg():");
  print_object_value(pop(pstack));

  puts("\nAdding -327.98 to stack...");
  object = new_object("-327.98");
  push(pstack, object);
  op_pos(pstack);
  puts("Value after calling op_pos():");
  print_object_value(pop(pstack));

  puts("\nTesting string object addition of: \"Greetings, \" + \"Concocter!\"");
  object = new_object("Concocter!");
  push(pstack, object);
  object = new_object("Greetings, ");
  push(pstack, object);
  op_add(pstack);
  puts("Result:");
  print_object_value(pop(pstack));

  puts("\nTesting string object multiplication of: \"foo\" * 3");
  object = new_object("foo");
  push(pstack, object);
  object = new_object("3");
  push(pstack, object);
  op_mul(pstack);
  puts("Result:");
  print_object_value(pop(pstack));

  puts("Executing NOP...");
  OP_NOOP;

  free_store();

  return 0;
}
