/*
 * Concoct - An imperative, dynamically-typed, interpreted, general-purpose programming language
 * Copyright (c) 2020-2021 BlakeTheBlock and Lloyd Dilley
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
  char* value = NULL;
  Object* object = NULL;

  // Below is just a demonstration for now
  vm.ip = vm.instructions;

  vm.instructions[11] = OP_RET;
  vm.instructions[10] = OP_BNT;
  push(vm.sp, new_object("32"));
  vm.instructions[9] = OP_XOR;
  push(vm.sp, new_object("32"));
  push(vm.sp, new_object("8"));
  vm.instructions[8] = OP_AND;
  push(vm.sp, new_object("true"));
  push(vm.sp, new_object("true"));
  vm.instructions[7] = OP_NEG;
  push(vm.sp, new_object("35.5"));
  vm.instructions[6] = OP_FLS;
  vm.instructions[5] = OP_NOP;
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
  vm.instructions[12] = OP_END;
  vm.ip = vm.instructions;

  while(*vm.ip != OP_END)
  {
    printf("Instruction: %s (0x%x)\n", get_mnemonic(*vm.ip), *vm.ip);
    switch((*vm.ip))
    {
      case OP_ADD:
        op_add(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_AND:
        op_and(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_BND:
        op_bnd(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_BNT:
        op_bnt(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_BOR:
        op_bor(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_CAL:
        break;
      case OP_CMP:
        break;
      case OP_DEC:
        op_dec(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_DIV:
        op_div(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_END:
        break;
      case OP_ENT:
        break;
      case OP_EQL:
        op_eql(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_EXT:
        break;
      case OP_FLS:
        op_fls(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_GT:
        op_gt(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_GTE:
        op_gte(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_HLT:
        break;
      case OP_INC:
        op_inc(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_JMC:
      case OP_JMP:
      case OP_JMZ:
      case OP_LNE:
      case OP_LNZ:
      case OP_LOE:
      case OP_LOP:
        break;
      case OP_LOZ:
        break;
      case OP_LT:
        op_lt(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_LTE:
        op_lte(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_MOD:
        op_mod(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_MOV:
        break;
      case OP_MUL:
        op_mul(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_NEG:
        op_neg(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_NEQ:
        op_neq(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_NOP:
        OP_NOOP;
        break;
      case OP_NOT:
        op_not(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_NUL:
        break;
      case OP_OR:
        op_or(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_POP:
        op_pop(vm.sp, object);
        print_object_value(pop(vm.sp));
        break;
      case OP_POS:
        op_pos(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_POW:
        op_pow(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_PSH:
        op_psh(vm.sp, value);
        print_object_value(pop(vm.sp));
        break;
      case OP_RET:
        break;
      case OP_SHL:
        op_shl(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_SHR:
        op_shr(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_SLE:
        op_sle(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_SLN:
        op_sln(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_STR:
        //op_str(vm.sp);
        //print_object_value(pop(vm.sp));
        break;
      case OP_SUB:
        op_sub(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_SYS:
        //op_sys(vm.sp);
        //print_object_value(pop(vm.sp));
        break;
      case OP_TRU:
        op_tru(vm.sp);
        print_object_value(pop(vm.sp));
        break;
      case OP_TST:
        break;
      case OP_XOR:
        op_xor(vm.sp);
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
