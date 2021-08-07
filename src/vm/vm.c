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
#include <stdio.h>  // fprintf(), printf()
#include <stdlib.h> // malloc()
#include <string.h> // strerror()
#include "debug.h"
#include "memory.h"
#include "vm/instructions.h"
#include "vm/opcodes.h"
#include "vm/vm.h"

VM vm;

// Initializes virtual machine
void init_vm()
{
  vm.instructions = (Opcode *)malloc(INSTRUCTION_STORE_SIZE * sizeof(Opcode));
  if(vm.instructions == NULL)
  {
    fprintf(stderr, "Error allocating memory for instruction store: %s\n", strerror(errno));
    return;
  }
  vm.sp = &vm.stack;
  init_stack(vm.sp);
  init_store();
  if(debug_mode)
    debug_print("VM initialized.");
  return;
}

// Stops virtual machine
void stop_vm()
{
  free(vm.instructions);
  free_store();
  if(debug_mode)
    debug_print("VM stopped.");
  return;
}

// Interprets code
RunCode interpret()
{
  init_vm();
  vm.sp = &vm.stack;

  // Below is just a demonstration for now
  vm.ip = vm.instructions;
  vm.instructions[4] = OP_ADD;
  push(vm.sp, new_object("2"));
  push(vm.sp, new_object("3"));
  vm.instructions[3] = OP_SUB;
  push(vm.sp, new_object("10"));
  push(vm.sp, new_object("3"));
  vm.instructions[2] = OP_MUL;
  push(vm.sp, new_object("5"));
  push(vm.sp, new_object("2"));
  vm.instructions[1] = OP_DEC;
  push(vm.sp, new_object("99"));
  vm.instructions[0] = OP_POW;
  push(vm.sp, new_object("5"));
  push(vm.sp, new_object("2"));
  vm.instructions[5] = OP_END;
  vm.ip = vm.instructions;

  while(vm.sp->count > 0 && *vm.ip != OP_END)
  {
    printf("Instruction: %s (0x%x)\n", get_mnemonic(*vm.ip), *vm.ip);
    switch((*vm.ip))
    {
      case OP_DEC:
        op_dec(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_INC:
        op_inc(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_ADD:
        op_add(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_SUB:
        op_sub(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_DIV:
        op_div(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_MUL:
        op_mul(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_MOD:
        op_mod(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_POW:
        op_pow(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      default:
        fprintf(stderr, "Illegal instruction: %s (0x%x)\n", get_mnemonic(*vm.ip), *vm.ip);
        return RUN_ERROR;
        break;
    }
    (*vm.ip++);
  }
  stop_vm();
  return RUN_SUCCESS;
}
