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
#include <inttypes.h> // PRIXPTR
#include <stdio.h>    // fprintf(), printf()
#include <stdlib.h>   // malloc()
#include <string.h>   // memset(), strcmp(), strerror()
#include "debug.h"
#include "memory.h"
#include "vm/instructions.h"
#include "vm/opcodes.h"
#include "vm/vm.h"

VM vm;
Opcode** IP;
Object** RP;
Stack** SP;

// Initializes virtual machine
void init_vm(void)
{
  memset(vm.registers, 0, sizeof(vm.registers));
  vm.instructions = (Opcode *)malloc(INSTRUCTION_STORE_SIZE * sizeof(Opcode));
  if(vm.instructions == NULL)
  {
    fprintf(stderr, "Error allocating memory for instruction store: %s\n", strerror(errno));
    return;
  }
  clear_instructions();
  vm.ip = vm.instructions;
  vm.rp = vm.registers;
  vm.sp = &vm.stack;
  IP = &vm.ip;
  RP = vm.rp;
  SP = &vm.sp;
  init_stack(vm.sp);
  init_store();
  if(debug_mode)
    debug_print("VM initialized.");
  return;
}

// Stops virtual machine
void stop_vm(void)
{
  free(vm.instructions);
  free_store();
  if(debug_mode)
    debug_print("VM stopped.");
  return;
}

// Clears instructions
void clear_instructions(void)
{
  for(size_t i = 0; i < INSTRUCTION_STORE_SIZE; i++)
    vm.instructions[i] = 0xFF;
}

// Reverses instructions since they should be in a LIFO arrangement
void reverse_instructions(size_t ic)
{
  if(ic > 0)
  {
    size_t i = ic - 1;
    size_t j = 0;
    while(i > j)
    {
      Opcode tmp = vm.instructions[i];
      vm.instructions[i] = vm.instructions[j];
      vm.instructions[j] = tmp;
      i--;
      j++;
    }
  }
}

// Prints register values
void print_registers(void)
{
  char* strval = NULL;
  void* vptr = NULL;
  Object* object = NULL;

  puts("Register values:");
  for(uint8_t i = 0; i < REGISTER_AMOUNT; i++)
  {
    object = vm.registers[i];
    if(object == NULL)
    {
      strval = NULL;
    }
    else
    {
      switch(object->datatype)
      {
        case CCT_TYPE_NIL:
          strval = "null";
          break;
        case CCT_TYPE_BOOL:
          vptr = &object->value.boolval;
          stringify(&strval, vptr, CCT_TYPE_BOOL);
          break;
        case CCT_TYPE_BYTE:
          vptr = &object->value.byteval;
          stringify(&strval, vptr, CCT_TYPE_BYTE);
          break;
        case CCT_TYPE_NUMBER:
          vptr = &object->value.numval;
          stringify(&strval, vptr, CCT_TYPE_NUMBER);
          break;
        case CCT_TYPE_BIGNUM:
          vptr = &object->value.bignumval;
          stringify(&strval, vptr, CCT_TYPE_BIGNUM);
          break;
        case CCT_TYPE_DECIMAL:
          vptr = &object->value.decimalval;
          stringify(&strval, vptr, CCT_TYPE_DECIMAL);
          break;
        case CCT_TYPE_STRING:
          vptr = &object->value.strobj.strval;
          stringify(&strval, vptr, CCT_TYPE_STRING);
          break;
        default:
          strval = "Unknown";
          break;
      }
    }

    if(i == REGISTER_AMOUNT - 1)
    {
      if(strval == NULL)
        printf("RS: empty\n");
      else
        printf("RS: %.64s (%s)\n", strval, get_data_type(object));
    }
    else
    {
      if(strval == NULL)
        printf("R%u: empty\n", i);
      else
        printf("R%u: %.64s (%s)\n", i, strval, get_data_type(object));
    }

    if(strval)
      free(strval);
  }

  if((*SP)->count > 0)
  {
    object = (*SP)->objects[(*SP)->top];
    switch(object->datatype)
    {
      case CCT_TYPE_NIL:
        strval = "null";
        break;
      case CCT_TYPE_BOOL:
        vptr = &object->value.boolval;
        stringify(&strval, vptr, CCT_TYPE_BOOL);
        break;
      case CCT_TYPE_BYTE:
        vptr = &object->value.byteval;
        stringify(&strval, vptr, CCT_TYPE_BYTE);
        break;
      case CCT_TYPE_NUMBER:
        vptr = &object->value.numval;
        stringify(&strval, vptr, CCT_TYPE_NUMBER);
        break;
      case CCT_TYPE_BIGNUM:
        vptr = &object->value.bignumval;
        stringify(&strval, vptr, CCT_TYPE_BIGNUM);
        break;
      case CCT_TYPE_DECIMAL:
        vptr = &object->value.decimalval;
        stringify(&strval, vptr, CCT_TYPE_DECIMAL);
        break;
      case CCT_TYPE_STRING:
        vptr = &object->value.strobj.strval;
        stringify(&strval, vptr, CCT_TYPE_STRING);
        break;
      default:
        strval = "Unknown";
        break;
    }
    printf("IP: %s (0x%02X)\nRP: 0x%" PRIXPTR "\nSP: %.64s (%s)\n\n", get_mnemonic(**IP), **IP, (uintptr_t)*RP, strval, get_data_type(object));
  }
  else
    printf("IP: %s (0x%02X)\nRP: 0x%" PRIXPTR "\nSP: empty\n\n", get_mnemonic(**IP), **IP, (uintptr_t)*RP);
  if(strval != NULL && (strcmp(strval, "null") != 0))
    free(strval);
  return;
}

// Interprets code
RunCode interpret(ConcoctHashMap* map)
{
  char* value = NULL;
  Object* object = NULL;
  Byte src_reg = R1;
  Byte dst_reg = R0;

  while(*vm.ip != OP_END)
  {
    if(debug_mode)
      printf("Instruction: %s (0x%02X)\n", get_mnemonic(*vm.ip), *vm.ip);
    switch(*vm.ip)
    {
      case OP_ADD:
        op_add(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_AND:
        op_and(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_ASN:
        op_asn(vm.sp, map);
        break;
      case OP_BND:
        op_bnd(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_BNT:
        op_bnt(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_BOR:
        op_bor(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_CAL:
        break;
      case OP_CLR:
        op_clr(vm.rp);
        if(debug_mode)
          print_registers();
        break;
      case OP_CLS:
        op_cls(vm.sp);
        break;
      case OP_CMP:
        break;
      case OP_DEC:
        op_dec(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_DIV:
        op_div(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_END:
        if(debug_mode)
          print_registers();
        break;
      case OP_ENT:
        break;
      case OP_EQL:
        op_eql(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_EXT:
        break;
      case OP_GT:
        op_gt(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_GTE:
        op_gte(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_HLT:
        stop_vm();
        break;
      case OP_INC:
        op_inc(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
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
          print_registers();
        break;
      case OP_LOE:
      case OP_LOP:
        break;
      case OP_LOZ:
        break;
      case OP_LT:
        op_lt(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_LTE:
        op_lte(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_MOD:
        op_mod(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_MOV:
        op_mov(vm.rp, object, src_reg, dst_reg);
        if(debug_mode)
          print_registers();
        break;
      case OP_MUL:
        op_mul(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_NEG:
        op_neg(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_NEQ:
        op_neq(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_NOP:
        OP_NOOP;
        break;
      case OP_NOT:
        op_not(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_NUL:
        break;
      case OP_OR:
        op_or(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_POP:
        op_pop(vm.sp, object);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_POS:
        op_pos(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_POW:
        op_pow(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_PSH:
        op_psh(vm.sp, value);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_RET:
        break;
      case OP_SHL:
        op_shl(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_SHR:
        op_shr(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_SLE:
        op_sle(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_SLN:
        op_sln(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_STR:
        op_str(vm.rp, vm.sp, src_reg);
        if(debug_mode)
        {
          print_registers();
          print_object_value(peek(vm.sp));
        }
        break;
      case OP_SUB:
        op_sub(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      case OP_SYS:
        //op_sys(vm.sp);
        //if(debug_mode)
        //  print_object_value(pop(vm.sp));
        break;
      case OP_TST:
        break;
      case OP_XCG:
        op_xcg(vm.rp, src_reg, dst_reg);
        if(debug_mode)
          print_registers();
        break;
      case OP_XOR:
        op_xor(vm.sp);
        if(debug_mode)
          print_object_value(peek(vm.sp));
        break;
      default:
        fprintf(stderr, "Illegal instruction: %s (0x%02X)\n", get_mnemonic(*vm.ip), *vm.ip);
        return RUN_ERROR;
    }
    (vm.ip)++;
  }

  if(debug_mode)
    print_registers();

  vm.ip = vm.instructions; // reset VM instruction pointer to beginning of instructions
  clear_instructions();

  return RUN_SUCCESS;
}
