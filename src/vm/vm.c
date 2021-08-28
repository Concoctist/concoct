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
#include <string.h> // memset(), strerror()
#include "debug.h"
#include "memory.h"
#include "vm/instructions.h"
#include "vm/opcodes.h"
#include "vm/vm.h"

VM vm;

// Initializes virtual machine
void init_vm()
{
  memset(vm.registers, 0, sizeof(vm.registers));
  vm.instructions = (Opcode *)malloc(INSTRUCTION_STORE_SIZE * sizeof(Opcode));
  if(vm.instructions == NULL)
  {
    fprintf(stderr, "Error allocating memory for instruction store: %s\n", strerror(errno));
    return;
  }
  vm.ip = vm.instructions;
  vm.rp = vm.registers;
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

// Prints register values
void print_registers()
{
  puts("Register values:");
  for(uint8_t i = 0; i < REGISTER_AMOUNT; i++)
  {
    if(i == REGISTER_AMOUNT / 2)
      puts("");
    if(i < 10)
      printf("R%u: %c0x%08lX   ", i, "+-"[(BigNum)vm.registers[i] < 0], (BigNum)abs(vm.registers[i]));
    else
      printf("R%u: %c0x%08lX  ", i, "+-"[(BigNum)vm.registers[i] < 0], (BigNum)abs(vm.registers[i]));
  }
  puts("");
  return;
}

// Interprets code
RunCode interpret()
{
  // Below is just a demonstration for now...
  BigNum numval = 53721;
  char* value = NULL;
  Object* object = NULL;
  Byte src_reg = R1;
  Byte dst_reg = R0;

  while(*vm.ip != OP_END)
  {
    if(debug_mode)
      printf("Instruction: %s (0x%02X)\n", get_mnemonic(*vm.ip), *vm.ip);
    switch((*vm.ip))
    {
      case OP_ADD:
        op_add(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_AND:
        op_and(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_BND:
        op_bnd(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_BNT:
        op_bnt(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_BOR:
        op_bor(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_CAL:
        break;
      case OP_CMP:
        break;
      case OP_DEC:
        op_dec(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_DIV:
        op_div(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_END:
        break;
      case OP_ENT:
        break;
      case OP_EQL:
        op_eql(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_EXT:
        break;
      case OP_FLS:
        op_fls(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_GT:
        op_gt(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_GTE:
        op_gte(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_HLT:
        stop_vm();
        break;
      case OP_INC:
        op_inc(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_JMC:
      case OP_JMP:
      case OP_JMZ:
      case OP_LNE:
      case OP_LNZ:
        break;
      case OP_LOD:
        op_lod(vm.rp, vm.sp, dst_reg);
        if(debug_mode)
        {
          print_registers();
        }
        break;
      case OP_LOE:
      case OP_LOP:
        break;
      case OP_LOZ:
        break;
      case OP_LT:
        op_lt(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_LTE:
        op_lte(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_MOD:
        op_mod(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_MOV:
        op_mov(vm.rp, numval, src_reg, dst_reg);
        if(debug_mode)
          print_registers();
        break;
      case OP_MUL:
        op_mul(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_NEG:
        op_neg(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_NEQ:
        op_neq(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_NOP:
        OP_NOOP;
        break;
      case OP_NOT:
        op_not(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_NUL:
        break;
      case OP_OR:
        op_or(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_POP:
        op_pop(vm.sp, object);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_POS:
        op_pos(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_POW:
        op_pow(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_PSH:
        op_psh(vm.sp, value);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_RET:
        break;
      case OP_SHL:
        op_shl(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_SHR:
        op_shr(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_SLE:
        op_sle(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_SLN:
        op_sln(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_STR:
        op_str(vm.rp, vm.sp, src_reg);
        if(debug_mode)
        {
          print_registers();
          print_object_value(pop(vm.sp));
        }
        break;
      case OP_SUB:
        op_sub(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_SYS:
        //op_sys(vm.sp);
        //if(debug_mode)
        //  print_object_value(pop(vm.sp));
        break;
      case OP_TRU:
        op_tru(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      case OP_TST:
        break;
      case OP_XOR:
        op_xor(vm.sp);
        if(debug_mode)
          print_object_value(pop(vm.sp));
        break;
      default:
        fprintf(stderr, "Illegal instruction: %s (0x%02X)\n", get_mnemonic(*vm.ip), *vm.ip);
        return RUN_ERROR;
        break;
    }
    (*vm.ip++);
  }
  stop_vm();
  return RUN_SUCCESS;
}
