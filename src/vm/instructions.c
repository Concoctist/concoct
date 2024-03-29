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

#include <math.h>            // pow()
#include <stdio.h>           // fprintf(), stderr
#include <stdlib.h>          // malloc()
#include <string.h>          // strcat(), strcmp()
#include "concoct.h"
#include "debug.h"
#include "memory.h"
#include "vm/instructions.h"
#include "vm/vm.h"

// Validates unary operand
RunCode unary_operand_check(const Object* operand, char* operator)
{
  if(operand->datatype == CCT_TYPE_NIL)
  {
    fprintf(stderr, "Invalid operation (%s) for object of type \"null\"!\n", operator);
    return RUN_ERROR;
  }
  if(operand->datatype == CCT_TYPE_BOOL)
  {
    fprintf(stderr, "Invalid operation (%s) for object of type \"boolean\"!\n", operator);
    return RUN_ERROR;
  }
  if(operand->datatype == CCT_TYPE_STRING)
  {
    fprintf(stderr, "Invalid operation (%s) for object of type \"string\"!\n", operator);
    return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Validates binary operands
RunCode binary_operand_check(const Object* operand1, const Object* operand2, char* operator)
{
  if(operand1->datatype == CCT_TYPE_NIL || operand2->datatype == CCT_TYPE_NIL)
  {
    fprintf(stderr, "Invalid operation (%s) for object of type \"null\"!\n", operator);
    return RUN_ERROR;
  }
  if(operand1->datatype == CCT_TYPE_BOOL || operand2->datatype == CCT_TYPE_BOOL)
  {
    fprintf(stderr, "Invalid operation (%s) for object of type \"boolean\"!\n", operator);
    return RUN_ERROR;
  }
  if(operand1->datatype == CCT_TYPE_STRING || operand2->datatype == CCT_TYPE_STRING)
  {
    fprintf(stderr, "Invalid operation (%s) for object of type \"string\"!\n", operator);
    return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Validates binary operands for operations that allow a pair of strings (+ and *)
RunCode binary_operand_check_str(const Object* operand1, const Object* operand2, char* operator)
{
  if(operand1->datatype == CCT_TYPE_NIL || operand2->datatype == CCT_TYPE_NIL)
  {
    fprintf(stderr, "Invalid operation (%s) for object of type \"null\"!\n", operator);
    return RUN_ERROR;
  }
  if(operand1->datatype == CCT_TYPE_BOOL || operand2->datatype == CCT_TYPE_BOOL)
  {
    fprintf(stderr, "Invalid operation (%s) for object of type \"boolean\"!\n", operator);
    return RUN_ERROR;
  }
  if((operand1->datatype == CCT_TYPE_STRING && (operand2->datatype != CCT_TYPE_NUMBER && operand2->datatype != CCT_TYPE_STRING)) || (operand2->datatype == CCT_TYPE_STRING && (operand1->datatype != CCT_TYPE_NUMBER && operand1->datatype != CCT_TYPE_STRING)))
  {
    fprintf(stderr, "Invalid binary operation (%s) for object of type \"string\"!\n", operator);
    return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Clear registers
RunCode op_clr(Object** rp)
{
  for(uint8_t i = 0; i < REGISTER_AMOUNT; i++)
    rp[i] = NULL;
  return RUN_SUCCESS;
}

// Clear stack
RunCode op_cls(Stack* stack)
{
  const Object* object = NULL;
  UNUSED(object);
  while(stack->count > 0)
    object = pop(stack);
  return RUN_SUCCESS;
}

// Load (load from memory to register)
RunCode op_lod(Object** rp, Stack* stack, Byte dst_reg)
{
  Object* object = pop(stack);
  if(object == NULL)
  {
    fprintf(stderr, "Object is NULL during LOD operation.\n");
    return RUN_ERROR;
  }
  if(dst_reg >= REGISTER_AMOUNT)
  {
    fprintf(stderr, "Invalid register during LOD operation.\n");
    return RUN_ERROR;
  }
  rp[dst_reg] = object;
  return RUN_SUCCESS;
}

// Move (move from register to register)
RunCode op_mov(Object** rp, Object* object, Byte src_reg, Byte dst_reg)
{
  if(dst_reg >= REGISTER_AMOUNT)
  {
    fprintf(stderr, "Invalid register during MOV operation.\n");
    return RUN_ERROR;
  }
  if(src_reg >= REGISTER_AMOUNT)
    rp[dst_reg] = object;
  else
    rp[dst_reg] = rp[src_reg];
  return RUN_SUCCESS;
}

// Store (store to memory from register)
RunCode op_str(Object** rp, Stack* stack, Byte src_reg)
{
  if(src_reg >= REGISTER_AMOUNT)
  {
    fprintf(stderr, "Invalid register during STR operation.\n");
    return RUN_ERROR;
  }
  push(stack, rp[src_reg]);
  return RUN_SUCCESS;
}

// Exchange/swap
RunCode op_xcg(Object** rp, Byte reg1, Byte reg2)
{
  Object* tmp = NULL;
  tmp = rp[reg1];
  rp[reg1] = rp[reg2];
  rp[reg2] = tmp;
  return RUN_SUCCESS;
}

// Pop
RunCode op_pop(Stack* stack, const Object* object)
{
  UNUSED(object);
  object = pop(stack);
  return RUN_SUCCESS;
}

// Push
RunCode op_psh(Stack* stack, char* value)
{
  Object* operand = new_object(value);
  if(operand == NULL)
  {
    fprintf(stderr, "Operand is NULL during PSH operation.\n");
    return RUN_ERROR;
  }
  push(stack, operand);
  return RUN_SUCCESS;
}

// Assign (=)
RunCode op_asn(Stack* stack, ConcoctHashMap* map)
{
  Object* key = pop(stack); // identifier used as a key
  Object* val = pop(stack); // value
  if(key == NULL || key->datatype != CCT_TYPE_STRING)
  {
    fprintf(stderr, "Identifier is not a string that can be used as a key during ASN operation.\n");
    return RUN_ERROR;
  }
  if(val == NULL)
  {
    fprintf(stderr, "Value is NULL during ASN operation.\n");
    return RUN_ERROR;
  }
  cct_hash_map_set(map, key->value.strobj.strval, val);
  if(debug_mode)
    print_object_value((Object *)cct_hash_map_get(map, key->value.strobj.strval));
  key->is_flagged = true; // throw away the key since we have it in the map
  return RUN_SUCCESS;
}

// Logical and (&&)
RunCode op_and(Stack* stack)
{
  Object* operand1 = pop(stack);
  Object* operand2 = pop(stack);
  Bool result = false;
  void* vptr = NULL;

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during AND operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during AND operation.\n");
    return RUN_ERROR;
  }

  if(operand1->datatype != CCT_TYPE_BOOL && operand2->datatype != CCT_TYPE_BOOL)
  {
    fprintf(stderr, "Invalid operation (&&) for non-bool object!\n");
    return RUN_ERROR;
  }

  result = *(Bool *)get_object_value(operand1) && *(Bool *)get_object_value(operand2);
  vptr = &result;
  push(stack, new_object_by_type(vptr, CCT_TYPE_BOOL));

  return RUN_SUCCESS;
}

// Logical not/negation (!)
RunCode op_not(Stack* stack)
{
  Object* operand = pop(stack);
  Bool result = false;
  void* vptr = NULL;

  if(operand == NULL)
  {
    fprintf(stderr, "Operand is NULL during NOT operation.\n");
    return RUN_ERROR;
  }

  if(operand->datatype != CCT_TYPE_BOOL)
  {
    fprintf(stderr, "Invalid operation (!) for non-bool object!\n");
    return RUN_ERROR;
  }

  result = *(Bool *)get_object_value(operand);
  result = !result;
  vptr = &result;
  push(stack, new_object_by_type(vptr, CCT_TYPE_BOOL));

  return RUN_SUCCESS;
}

// Logical or (||)
RunCode op_or(Stack* stack)
{
  Object* operand1 = pop(stack);
  Object* operand2 = pop(stack);
  Bool result = false;
  void* vptr = NULL;

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during OR operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during OR operation.\n");
    return RUN_ERROR;
  }

  if(operand1->datatype != CCT_TYPE_BOOL && operand2->datatype != CCT_TYPE_BOOL)
  {
    fprintf(stderr, "Invalid operation (||) for non-bool object!\n");
    return RUN_ERROR;
  }

  result = *(Bool *)get_object_value(operand1) || *(Bool *)get_object_value(operand2);
  vptr = &result;
  push(stack, new_object_by_type(vptr, CCT_TYPE_BOOL));

  return RUN_SUCCESS;
}

// Equal to (==)
RunCode op_eql(Stack* stack)
{
  Object* operand1 = pop(stack);
  Object* operand2 = pop(stack);

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during EQL operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during EQL operation.\n");
    return RUN_ERROR;
  }

  if(operand1->datatype == CCT_TYPE_NIL && operand2->datatype == CCT_TYPE_NIL)
  {
    push(stack, new_object("true"));
    return RUN_SUCCESS;
  }

  if(operand1->datatype == CCT_TYPE_BOOL && operand2->datatype == CCT_TYPE_BOOL)
  {
    if(operand1->value.boolval == operand2->value.boolval)
      push(stack, new_object("true"));
    else
      push(stack, new_object("false"));
    return RUN_SUCCESS;
  }

  if(operand1->datatype == CCT_TYPE_STRING && operand2->datatype == CCT_TYPE_STRING)
  {
    if(strcmp(operand1->value.strobj.strval, operand2->value.strobj.strval) == 0)
      push(stack, new_object("true"));
    else
      push(stack, new_object("false"));
    return RUN_SUCCESS;
  }

  switch(operand1->datatype)
  {
    case CCT_TYPE_BYTE:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(Byte *)get_object_value(operand1) == *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(Byte *)get_object_value(operand1) == *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(Byte *)get_object_value(operand1) == *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(Byte *)get_object_value(operand1) == *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (==)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_NUMBER:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(Number *)get_object_value(operand1) == *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(Number *)get_object_value(operand1) == *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(Number *)get_object_value(operand1) == *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(Number *)get_object_value(operand1) == *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (==)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_BIGNUM:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(BigNum *)get_object_value(operand1) == *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(BigNum *)get_object_value(operand1) == *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(BigNum *)get_object_value(operand1) == *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(BigNum *)get_object_value(operand1) == *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (==)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_DECIMAL:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(Decimal *)get_object_value(operand1) == *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(Decimal *)get_object_value(operand1) == *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(Decimal *)get_object_value(operand1) == *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(Decimal *)get_object_value(operand1) == *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (==)!\n");
          return RUN_ERROR;
      }
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (==)!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Not equal to (!=)
RunCode op_neq(Stack* stack)
{
  Object* operand1 = pop(stack);
  Object* operand2 = pop(stack);

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during NEQ operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during NEQ operation.\n");
    return RUN_ERROR;
  }

  if(operand1->datatype == CCT_TYPE_NIL && operand2->datatype == CCT_TYPE_NIL)
  {
    push(stack, new_object("false"));
    return RUN_SUCCESS;
  }

  if(operand1->datatype == CCT_TYPE_BOOL && operand2->datatype == CCT_TYPE_BOOL)
  {
    if(operand1->value.boolval != operand2->value.boolval)
      push(stack, new_object("true"));
    else
      push(stack, new_object("false"));
    return RUN_SUCCESS;
  }

  if(operand1->datatype == CCT_TYPE_STRING && operand2->datatype == CCT_TYPE_STRING)
  {
    if(strcmp(operand1->value.strobj.strval, operand2->value.strobj.strval) != 0)
      push(stack, new_object("true"));
    else
      push(stack, new_object("false"));
    return RUN_SUCCESS;
  }

  switch(operand1->datatype)
  {
    case CCT_TYPE_BYTE:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(Byte *)get_object_value(operand1) != *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(Byte *)get_object_value(operand1) != *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(Byte *)get_object_value(operand1) != *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(Byte *)get_object_value(operand1) != *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (!=)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_NUMBER:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(Number *)get_object_value(operand1) != *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(Number *)get_object_value(operand1) != *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(Number *)get_object_value(operand1) != *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(Number *)get_object_value(operand1) != *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (!=)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_BIGNUM:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(BigNum *)get_object_value(operand1) != *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(BigNum *)get_object_value(operand1) != *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(BigNum *)get_object_value(operand1) != *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(BigNum *)get_object_value(operand1) != *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (!=)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_DECIMAL:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(Decimal *)get_object_value(operand1) != *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(Decimal *)get_object_value(operand1) != *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(Decimal *)get_object_value(operand1) != *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(Decimal *)get_object_value(operand1) != *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (!=)!\n");
          return RUN_ERROR;
      }
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (!=)!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// String length equal to ($=)
RunCode op_sle(Stack* stack)
{
  const Object* operand1 = pop(stack);
  const Object* operand2 = pop(stack);

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during SEQ operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during SEQ operation.\n");
    return RUN_ERROR;
  }

  if(operand1->datatype == CCT_TYPE_STRING && operand2->datatype == CCT_TYPE_STRING)
  {
    if(operand1->value.strobj.length == operand2->value.strobj.length)
      push(stack, new_object("true"));
    else
      push(stack, new_object("false"));
    return RUN_SUCCESS;
  }
  else
  {
    fprintf(stderr, "Invalid operand type encountered during operation ($=)!\n");
    return RUN_ERROR;
  }
}

// String length not equal to ($!)
RunCode op_sln(Stack* stack)
{
  const Object* operand1 = pop(stack);
  const Object* operand2 = pop(stack);

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during SNE operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during SNE operation.\n");
    return RUN_ERROR;
  }

  if(operand1->datatype == CCT_TYPE_STRING && operand2->datatype == CCT_TYPE_STRING)
  {
    if(operand1->value.strobj.length != operand2->value.strobj.length)
      push(stack, new_object("true"));
    else
      push(stack, new_object("false"));
    return RUN_SUCCESS;
  }
  else
  {
    fprintf(stderr, "Invalid operand type encountered during operation ($!)!\n");
    return RUN_ERROR;
  }
}

// Greater than (>)
RunCode op_gt(Stack* stack)
{
  Object* operand1 = pop(stack);
  Object* operand2 = pop(stack);

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during GT operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during GT operation.\n");
    return RUN_ERROR;
  }

  if(binary_operand_check_str(operand1, operand2, ">") == RUN_ERROR)
    return RUN_ERROR;

  if(operand1->datatype == CCT_TYPE_STRING && operand2->datatype == CCT_TYPE_STRING)
  {
    if(operand1->value.strobj.length > operand2->value.strobj.length)
      push(stack, new_object("true"));
    else
      push(stack, new_object("false"));
    return RUN_SUCCESS;
  }

  switch(operand1->datatype)
  {
    case CCT_TYPE_BYTE:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(Byte *)get_object_value(operand1) > *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(Byte *)get_object_value(operand1) > *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(Byte *)get_object_value(operand1) > *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(Byte *)get_object_value(operand1) > *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (>)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_NUMBER:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(Number *)get_object_value(operand1) > *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(Number *)get_object_value(operand1) > *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(Number *)get_object_value(operand1) > *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(Number *)get_object_value(operand1) > *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (>)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_BIGNUM:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(BigNum *)get_object_value(operand1) > *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(BigNum *)get_object_value(operand1) > *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(BigNum *)get_object_value(operand1) > *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(BigNum *)get_object_value(operand1) > *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (>)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_DECIMAL:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(Decimal *)get_object_value(operand1) > *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(Decimal *)get_object_value(operand1) > *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(Decimal *)get_object_value(operand1) > *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(Decimal *)get_object_value(operand1) > *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (>)!\n");
          return RUN_ERROR;
      }
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (>)!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Greater than or equal to (>=)
RunCode op_gte(Stack* stack)
{
  Object* operand1 = pop(stack);
  Object* operand2 = pop(stack);

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during GTE operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during GTE operation.\n");
    return RUN_ERROR;
  }

  if(binary_operand_check_str(operand1, operand2, ">=") == RUN_ERROR)
    return RUN_ERROR;

  if(operand1->datatype == CCT_TYPE_STRING && operand2->datatype == CCT_TYPE_STRING)
  {
    if(operand1->value.strobj.length >= operand2->value.strobj.length)
      push(stack, new_object("true"));
    else
      push(stack, new_object("false"));
    return RUN_SUCCESS;
  }

  switch(operand1->datatype)
  {
    case CCT_TYPE_BYTE:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(Byte *)get_object_value(operand1) >= *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(Byte *)get_object_value(operand1) >= *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(Byte *)get_object_value(operand1) >= *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(Byte *)get_object_value(operand1) >= *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (>=)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_NUMBER:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(Number *)get_object_value(operand1) >= *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(Number *)get_object_value(operand1) >= *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(Number *)get_object_value(operand1) >= *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(Number *)get_object_value(operand1) >= *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (>=)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_BIGNUM:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(BigNum *)get_object_value(operand1) >= *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(BigNum *)get_object_value(operand1) >= *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(BigNum *)get_object_value(operand1) >= *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(BigNum *)get_object_value(operand1) >= *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (>=)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_DECIMAL:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(Decimal *)get_object_value(operand1) >= *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(Decimal *)get_object_value(operand1) >= *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(Decimal *)get_object_value(operand1) >= *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(Decimal *)get_object_value(operand1) >= *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (>=)!\n");
          return RUN_ERROR;
      }
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (>=)!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Less than (<)
RunCode op_lt(Stack* stack)
{
  Object* operand1 = pop(stack);
  Object* operand2 = pop(stack);

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during LT operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during LT operation.\n");
    return RUN_ERROR;
  }

  if(binary_operand_check_str(operand1, operand2, "<") == RUN_ERROR)
    return RUN_ERROR;

  if(operand1->datatype == CCT_TYPE_STRING && operand2->datatype == CCT_TYPE_STRING)
  {
    if(operand1->value.strobj.length < operand2->value.strobj.length)
      push(stack, new_object("true"));
    else
      push(stack, new_object("false"));
    return RUN_SUCCESS;
  }

  switch(operand1->datatype)
  {
    case CCT_TYPE_BYTE:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(Byte *)get_object_value(operand1) < *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(Byte *)get_object_value(operand1) < *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(Byte *)get_object_value(operand1) < *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(Byte *)get_object_value(operand1) < *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (<)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_NUMBER:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(Number *)get_object_value(operand1) < *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(Number *)get_object_value(operand1) < *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(Number *)get_object_value(operand1) < *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(Number *)get_object_value(operand1) < *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (<)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_BIGNUM:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(BigNum *)get_object_value(operand1) < *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(BigNum *)get_object_value(operand1) < *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(BigNum *)get_object_value(operand1) < *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(BigNum *)get_object_value(operand1) < *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (<)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_DECIMAL:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(Decimal *)get_object_value(operand1) < *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(Decimal *)get_object_value(operand1) < *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(Decimal *)get_object_value(operand1) < *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(Decimal *)get_object_value(operand1) < *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (<)!\n");
          return RUN_ERROR;
      }
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (<)!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Less than or equal to (<=)
RunCode op_lte(Stack* stack)
{
  Object* operand1 = pop(stack);
  Object* operand2 = pop(stack);

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during LTE operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during LTE operation.\n");
    return RUN_ERROR;
  }

  if(binary_operand_check_str(operand1, operand2, "<=") == RUN_ERROR)
    return RUN_ERROR;

  if(operand1->datatype == CCT_TYPE_STRING && operand2->datatype == CCT_TYPE_STRING)
  {
    if(operand1->value.strobj.length <= operand2->value.strobj.length)
      push(stack, new_object("true"));
    else
      push(stack, new_object("false"));
    return RUN_SUCCESS;
  }

  switch(operand1->datatype)
  {
    case CCT_TYPE_BYTE:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(Byte *)get_object_value(operand1) <= *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(Byte *)get_object_value(operand1) <= *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(Byte *)get_object_value(operand1) <= *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(Byte *)get_object_value(operand1) <= *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (<=)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_NUMBER:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(Number *)get_object_value(operand1) <= *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(Number *)get_object_value(operand1) <= *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(Number *)get_object_value(operand1) <= *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(Number *)get_object_value(operand1) <= *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (<=)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_BIGNUM:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(BigNum *)get_object_value(operand1) <= *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(BigNum *)get_object_value(operand1) <= *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(BigNum *)get_object_value(operand1) <= *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(BigNum *)get_object_value(operand1) <= *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (<=)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_DECIMAL:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          if(*(Decimal *)get_object_value(operand1) <= *(Byte *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_NUMBER:
          if(*(Decimal *)get_object_value(operand1) <= *(Number *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_BIGNUM:
          if(*(Decimal *)get_object_value(operand1) <= *(BigNum *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        case CCT_TYPE_DECIMAL:
          if(*(Decimal *)get_object_value(operand1) <= *(Decimal *)get_object_value(operand2))
            push(stack, new_object("true"));
          else
            push(stack, new_object("false"));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (<=)!\n");
          return RUN_ERROR;
      }
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (<=)!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Negative
RunCode op_neg(Stack* stack)
{
  Object* operand = pop(stack);
  Number numval = 0;
  BigNum bignumval = 0;
  Decimal decimalval = 0.0;
  void* vptr = NULL;

  if(operand == NULL)
  {
    fprintf(stderr, "Operand is NULL during NEG operation.\n");
    return RUN_ERROR;
  }

  switch(operand->datatype)
  {
    case CCT_TYPE_NUMBER:
      numval = *(Number *)get_object_value(operand);
      if(numval > 0)
        numval *= -1;
      vptr = &numval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
      break;
    case CCT_TYPE_BIGNUM:
      bignumval = *(BigNum *)get_object_value(operand);
      if(bignumval > 0)
        bignumval *= -1;
      vptr = &bignumval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
      break;
    case CCT_TYPE_DECIMAL:
      decimalval = *(Decimal *)get_object_value(operand);
      if(decimalval > 0)
        decimalval *= -1;
      vptr = &decimalval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during NEG operation!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Positive
RunCode op_pos(Stack* stack)
{
  Object* operand = pop(stack);
  Number numval = 0;
  BigNum bignumval = 0;
  Decimal decimalval = 0.0;
  void* vptr = NULL;

  if(operand == NULL)
  {
    fprintf(stderr, "Operand is NULL during POS operation.\n");
    return RUN_ERROR;
  }

  switch(operand->datatype)
  {
    case CCT_TYPE_NUMBER:
      numval = *(Number *)get_object_value(operand);
      if(numval < 0)
        numval *= -1;
      vptr = &numval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
      break;
    case CCT_TYPE_BIGNUM:
      bignumval = *(BigNum *)get_object_value(operand);
      if(bignumval < 0)
        bignumval *= -1;
      vptr = &bignumval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
      break;
    case CCT_TYPE_DECIMAL:
      decimalval = *(Decimal *)get_object_value(operand);
      if(decimalval < 0)
        decimalval *= -1;
      vptr = &decimalval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during POS operation!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Decrement (--)
RunCode op_dec(Stack* stack)
{
  Object* operand = pop(stack);
  Byte byteval = 0;
  Number numval = 0;
  BigNum bignumval = 0;
  Decimal decimalval = 0.0;
  void* vptr = NULL;

  if(operand == NULL)
  {
    fprintf(stderr, "Operand is NULL during DEC operation.\n");
    return RUN_ERROR;
  }

  if(unary_operand_check(operand, "--") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand->datatype)
  {
    case CCT_TYPE_BYTE:
      byteval = *(Byte *)get_object_value(operand);
      byteval--;
      vptr = &byteval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_BYTE));
      break;
    case CCT_TYPE_NUMBER:
      numval = *(Number *)get_object_value(operand);
      numval--;
      vptr = &numval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
      break;
    case CCT_TYPE_BIGNUM:
      bignumval = *(BigNum *)get_object_value(operand);
      bignumval--;
      vptr = &bignumval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
      break;
    case CCT_TYPE_DECIMAL:
      decimalval = *(Decimal *)get_object_value(operand);
      decimalval--;
      vptr = &decimalval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (--)!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Increment (++)
RunCode op_inc(Stack* stack)
{
  Object* operand = pop(stack);
  Byte byteval = 0;
  Number numval = 0;
  BigNum bignumval = 0;
  Decimal decimalval = 0.0;
  void* vptr = NULL;

  if(operand == NULL)
  {
    fprintf(stderr, "Operand is NULL during INC operation.\n");
    return RUN_ERROR;
  }

  if(unary_operand_check(operand, "++") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand->datatype)
  {
    case CCT_TYPE_BYTE:
      byteval = *(Byte *)get_object_value(operand);
      byteval++;
      vptr = &byteval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_BYTE));
      break;
    case CCT_TYPE_NUMBER:
      numval = *(Number *)get_object_value(operand);
      numval++;
      vptr = &numval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
      break;
    case CCT_TYPE_BIGNUM:
      bignumval = *(BigNum *)get_object_value(operand);
      bignumval++;
      vptr = &bignumval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
      break;
    case CCT_TYPE_DECIMAL:
      decimalval = *(Decimal *)get_object_value(operand);
      decimalval++;
      vptr = &decimalval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (++)!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Addition (+)
RunCode op_add(Stack* stack)
{
  Object* operand1 = pop(stack); // augend
  Object* operand2 = pop(stack); // addend
  Byte byteval = 0;
  Number numval = 0;
  BigNum bignumval = 0;
  Decimal decimalval = 0.0;
  char* addstr = NULL;
  void* vptr = NULL;

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during ADD operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during ADD operation.\n");
    return RUN_ERROR;
  }

  if(binary_operand_check_str(operand1, operand2, "+") == RUN_ERROR)
    return RUN_ERROR;

  if(operand1->datatype == CCT_TYPE_STRING && operand2->datatype == CCT_TYPE_STRING) // string concatenation
  {
    addstr = malloc(operand1->value.strobj.length + operand2->value.strobj.length + 1);
    if(addstr == NULL)
    {
      fprintf(stderr, "Unable to allocate memory for string during ADD operation.\n");
      return RUN_ERROR;
    }
    strcpy(addstr, operand1->value.strobj.strval);
    push(stack, new_object(strcat(addstr, operand2->value.strobj.strval)));
    free(addstr);
  }
  else
  {
    switch(operand1->datatype)
    {
      case CCT_TYPE_BYTE:
        switch(operand2->datatype)
        {
          case CCT_TYPE_BYTE:
            byteval = *(Byte *)get_object_value(operand1) + *(Byte *)get_object_value(operand2);
            vptr = &byteval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_BYTE));
            break;
          case CCT_TYPE_NUMBER:
            numval = *(Byte *)get_object_value(operand1) + *(Number *)get_object_value(operand2);
            vptr = &numval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
            break;
          case CCT_TYPE_BIGNUM:
            bignumval = *(Byte *)get_object_value(operand1) + *(BigNum *)get_object_value(operand2);
            vptr = &bignumval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
            break;
          case CCT_TYPE_DECIMAL:
            decimalval = *(Byte *)get_object_value(operand1) + *(Decimal *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
            break;
          default:
            fprintf(stderr, "Invalid operand type encountered during operation (+)!\n");
            return RUN_ERROR;
        }
        break;
      case CCT_TYPE_NUMBER:
        switch(operand2->datatype)
        {
          case CCT_TYPE_BYTE:
            numval = *(Number *)get_object_value(operand1) + *(Byte *)get_object_value(operand2);
            vptr = &numval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
            break;
          case CCT_TYPE_NUMBER:
            numval = *(Number *)get_object_value(operand1) + *(Number *)get_object_value(operand2);
            vptr = &numval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
            break;
          case CCT_TYPE_BIGNUM:
            bignumval = *(Number *)get_object_value(operand1) + *(BigNum *)get_object_value(operand2);
            vptr = &bignumval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
            break;
          case CCT_TYPE_DECIMAL:
            decimalval = *(Number *)get_object_value(operand1) + *(Decimal *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
            break;
          default:
            fprintf(stderr, "Invalid operand type encountered during operation (+)!\n");
            return RUN_ERROR;
        }
        break;
      case CCT_TYPE_BIGNUM:
        switch(operand2->datatype)
        {
          case CCT_TYPE_BYTE:
            bignumval = *(BigNum *)get_object_value(operand1) + *(Byte *)get_object_value(operand2);
            vptr = &bignumval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
            break;
          case CCT_TYPE_NUMBER:
            bignumval = *(BigNum *)get_object_value(operand1) + *(Number *)get_object_value(operand2);
            vptr = &bignumval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
            break;
          case CCT_TYPE_BIGNUM:
            bignumval = *(BigNum *)get_object_value(operand1) + *(BigNum *)get_object_value(operand2);
            vptr = &bignumval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
            break;
          case CCT_TYPE_DECIMAL:
            decimalval = *(BigNum *)get_object_value(operand1) + *(Decimal *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
            break;
          default:
            fprintf(stderr, "Invalid operand type encountered during operation (+)!\n");
            return RUN_ERROR;
        }
        break;
      case CCT_TYPE_DECIMAL:
        switch(operand2->datatype)
        {
          case CCT_TYPE_BYTE:
            decimalval = *(Decimal *)get_object_value(operand1) + *(Byte *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
            break;
          case CCT_TYPE_NUMBER:
            decimalval = *(Decimal *)get_object_value(operand1) + *(Number *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
            break;
          case CCT_TYPE_BIGNUM:
            decimalval = *(Decimal *)get_object_value(operand1) + *(BigNum *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
            break;
          case CCT_TYPE_DECIMAL:
            decimalval = *(Decimal *)get_object_value(operand1) + *(Decimal *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
            break;
          default:
            fprintf(stderr, "Invalid operand type encountered during operation (+)!\n");
            return RUN_ERROR;
        }
        break;
      default:
        fprintf(stderr, "Invalid operand type encountered during operation (+)!\n");
        return RUN_ERROR;
    }
  }
  return RUN_SUCCESS;
}

// Subtraction (-)
RunCode op_sub(Stack* stack)
{
  Object* operand1 = pop(stack); // minuend
  Object* operand2 = pop(stack); // subtrahend
  Byte byteval = 0;
  Number numval = 0;
  BigNum bignumval = 0;
  Decimal decimalval = 0.0;
  void* vptr = NULL;

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during SUB operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during SUB operation.\n");
    return RUN_ERROR;
  }

  if(binary_operand_check(operand1, operand2, "-") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand1->datatype)
  {
    case CCT_TYPE_BYTE:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          byteval = *(Byte *)get_object_value(operand1) - *(Byte *)get_object_value(operand2);
          vptr = &byteval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BYTE));
          break;
        case CCT_TYPE_NUMBER:
          numval = *(Byte *)get_object_value(operand1) - *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = *(Byte *)get_object_value(operand1) - *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = *(Byte *)get_object_value(operand1) - *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (-)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_NUMBER:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          numval = *(Number *)get_object_value(operand1) - *(Byte *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_NUMBER:
          numval = *(Number *)get_object_value(operand1) - *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = *(Number *)get_object_value(operand1) - *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = *(Number *)get_object_value(operand1) - *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (-)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_BIGNUM:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          bignumval = *(BigNum *)get_object_value(operand1) - *(Byte *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_NUMBER:
          bignumval = *(BigNum *)get_object_value(operand1) - *(Number *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = *(BigNum *)get_object_value(operand1) - *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = *(BigNum *)get_object_value(operand1) - *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (-)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_DECIMAL:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          decimalval = *(Decimal *)get_object_value(operand1) - *(Byte *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_NUMBER:
          decimalval = *(Decimal *)get_object_value(operand1) - *(Number *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_BIGNUM:
          decimalval = *(Decimal *)get_object_value(operand1) - *(BigNum *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = *(Decimal *)get_object_value(operand1) - *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (-)!\n");
          return RUN_ERROR;
      }
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (-)!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Division (/)
RunCode op_div(Stack* stack)
{
  Object* operand1 = pop(stack); // dividend
  Object* operand2 = pop(stack); // divisor
  Byte byteval = 0;
  Number numval = 0;
  BigNum bignumval = 0;
  Decimal decimalval = 0.0;
  void* vptr = NULL;
  Bool div_by_zero = false;

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during DIV operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during DIV operation.\n");
    return RUN_ERROR;
  }

  switch(operand2->datatype)
  {
    case CCT_TYPE_BYTE:
      if(*(Byte *)get_object_value(operand2) == 0)
        div_by_zero = true;
      break;
    case CCT_TYPE_NUMBER:
      if(*(Number *)get_object_value(operand2) == 0)
        div_by_zero = true;
      break;
    case CCT_TYPE_BIGNUM:
      if(*(BigNum *)get_object_value(operand2) == 0)
        div_by_zero = true;
      break;
    case CCT_TYPE_DECIMAL:
      if(*(Decimal *)get_object_value(operand2) == 0.0)
        div_by_zero = true;
      break;
    default:
      break;
  }
  if(div_by_zero)
  {
    fprintf(stderr, "Operand 2 is 0 (zero) during DIV operation.\n");
    return RUN_ERROR;
  }

  if(binary_operand_check(operand1, operand2, "/") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand1->datatype)
  {
    case CCT_TYPE_BYTE:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          byteval = *(Byte *)get_object_value(operand1) / *(Byte *)get_object_value(operand2);
          vptr = &byteval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BYTE));
          break;
        case CCT_TYPE_NUMBER:
          numval = *(Byte *)get_object_value(operand1) / *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = *(Byte *)get_object_value(operand1) / *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = *(Byte *)get_object_value(operand1) / *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (/)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_NUMBER:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          numval = *(Number *)get_object_value(operand1) / *(Byte *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_NUMBER:
          numval = *(Number *)get_object_value(operand1) / *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = *(Number *)get_object_value(operand1) / *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = *(Number *)get_object_value(operand1) / *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (/)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_BIGNUM:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          bignumval = *(BigNum *)get_object_value(operand1) / *(Byte *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_NUMBER:
          bignumval = *(BigNum *)get_object_value(operand1) / *(Number *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = *(BigNum *)get_object_value(operand1) / *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = *(BigNum *)get_object_value(operand1) / *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (/)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_DECIMAL:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          decimalval = *(Decimal *)get_object_value(operand1) / *(Byte *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_NUMBER:
          decimalval = *(Decimal *)get_object_value(operand1) / *(Number *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_BIGNUM:
          decimalval = *(Decimal *)get_object_value(operand1) / *(BigNum *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = *(Decimal *)get_object_value(operand1) / *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (/)!\n");
          return RUN_ERROR;
      }
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (/)!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Multiplication (*)
RunCode op_mul(Stack* stack)
{
  Object* operand1 = pop(stack); // multiplier
  Object* operand2 = pop(stack); // multiplicand
  Byte byteval = 0;
  Number numval = 0;
  BigNum bignumval = 0;
  Decimal decimalval = 0.0;
  char* multstr = NULL;
  void* vptr = NULL;

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during MUL operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during MUL operation.\n");
    return RUN_ERROR;
  }

  if(binary_operand_check_str(operand1, operand2, "*") == RUN_ERROR)
    return RUN_ERROR;

  // string multiplication
  if((operand1->datatype == CCT_TYPE_STRING && operand2->datatype == CCT_TYPE_NUMBER) || (operand1->datatype == CCT_TYPE_NUMBER && operand2->datatype == CCT_TYPE_STRING))
  {
    if(operand1->datatype == CCT_TYPE_STRING)
    {
      multstr = malloc(operand1->value.strobj.length * *(Number *)get_object_value(operand2) + *(Number *)get_object_value(operand2));
      if(multstr == NULL)
      {
        fprintf(stderr, "Unable to allocate memory for string during MUL operation.\n");
        return RUN_ERROR;
      }
      strcpy(multstr, operand1->value.strobj.strval);
      // start counter at 1 since we already called strcpy() above to null terminate multstr
      for(int i = 1; i < abs(*(Number *)get_object_value(operand2)); i++)
        strcat(multstr, operand1->value.strobj.strval);
    }
    else
    {
      multstr = malloc(operand2->value.strobj.length * *(Number *)get_object_value(operand1) + *(Number *)get_object_value(operand1));
      if(multstr == NULL)
      {
        fprintf(stderr, "Unable to allocate memory for string during MUL operation.\n");
        return RUN_ERROR;
      }
      strcpy(multstr, operand2->value.strobj.strval);
      // start counter at 1 since we already called strcpy() above to null terminate multstr
      for(int i = 1; i < abs(*(Number *)get_object_value(operand1)); i++)
        strcat(multstr, operand2->value.strobj.strval);
    }
    push(stack, new_object(multstr));
    free(multstr);
  }
  else
  {
    switch(operand1->datatype)
    {
      case CCT_TYPE_BYTE:
        switch(operand2->datatype)
        {
          case CCT_TYPE_BYTE:
            byteval = *(Byte *)get_object_value(operand1) * *(Byte *)get_object_value(operand2);
            vptr = &byteval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_BYTE));
            break;
          case CCT_TYPE_NUMBER:
            numval = *(Byte *)get_object_value(operand1) * *(Number *)get_object_value(operand2);
            vptr = &numval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
            break;
          case CCT_TYPE_BIGNUM:
            bignumval = *(Byte *)get_object_value(operand1) * *(BigNum *)get_object_value(operand2);
            vptr = &bignumval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
            break;
          case CCT_TYPE_DECIMAL:
            decimalval = *(Byte *)get_object_value(operand1) * *(Decimal *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
            break;
          default:
            fprintf(stderr, "Invalid operand type encountered during operation (*)!\n");
            return RUN_ERROR;
        }
        break;
      case CCT_TYPE_NUMBER:
        switch(operand2->datatype)
        {
          case CCT_TYPE_BYTE:
            numval = *(Number *)get_object_value(operand1) * *(Byte *)get_object_value(operand2);
            vptr = &numval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
            break;
          case CCT_TYPE_NUMBER:
            numval = *(Number *)get_object_value(operand1) * *(Number *)get_object_value(operand2);
            vptr = &numval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
            break;
          case CCT_TYPE_BIGNUM:
            bignumval = *(Number *)get_object_value(operand1) * *(BigNum *)get_object_value(operand2);
            vptr = &bignumval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
            break;
          case CCT_TYPE_DECIMAL:
            decimalval = *(Number *)get_object_value(operand1) * *(Decimal *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
            break;
          default:
            fprintf(stderr, "Invalid operand type encountered during operation (*)!\n");
            return RUN_ERROR;
        }
        break;
      case CCT_TYPE_BIGNUM:
        switch(operand2->datatype)
        {
          case CCT_TYPE_BYTE:
            bignumval = *(BigNum *)get_object_value(operand1) * *(Byte *)get_object_value(operand2);
            vptr = &bignumval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
            break;
          case CCT_TYPE_NUMBER:
            bignumval = *(BigNum *)get_object_value(operand1) * *(Number *)get_object_value(operand2);
            vptr = &bignumval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
            break;
          case CCT_TYPE_BIGNUM:
            bignumval = *(BigNum *)get_object_value(operand1) * *(BigNum *)get_object_value(operand2);
            vptr = &bignumval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
            break;
          case CCT_TYPE_DECIMAL:
            decimalval = *(BigNum *)get_object_value(operand1) * *(Decimal *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
            break;
          default:
            fprintf(stderr, "Invalid operand type encountered during operation (*)!\n");
            return RUN_ERROR;
        }
        break;
      case CCT_TYPE_DECIMAL:
        switch(operand2->datatype)
        {
          case CCT_TYPE_BYTE:
            decimalval = *(Decimal *)get_object_value(operand1) * *(Byte *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
            break;
          case CCT_TYPE_NUMBER:
            decimalval = *(Decimal *)get_object_value(operand1) * *(Number *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
            break;
          case CCT_TYPE_BIGNUM:
            decimalval = *(Decimal *)get_object_value(operand1) * *(BigNum *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
            break;
          case CCT_TYPE_DECIMAL:
            decimalval = *(Decimal *)get_object_value(operand1) * *(Decimal *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
            break;
          default:
            fprintf(stderr, "Invalid operand type encountered during operation (*)!\n");
            return RUN_ERROR;
        }
        break;
      default:
        fprintf(stderr, "Invalid operand type encountered during operation (*)!\n");
        return RUN_ERROR;
    }
  }
  return RUN_SUCCESS;
}

// Modulo (%)
// Note: Modulo operates on integers. Decimal numbers are truncated.
RunCode op_mod(Stack* stack)
{
  Object* operand1 = pop(stack); // dividend
  Object* operand2 = pop(stack); // divisor
  Byte byteval = 0;
  Number numval = 0;
  BigNum bignumval = 0;
  Decimal decimalval = 0.0;
  void* vptr = NULL;

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during MOD operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during MOD operation.\n");
    return RUN_ERROR;
  }

  if(binary_operand_check(operand1, operand2, "%") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand1->datatype)
  {
    case CCT_TYPE_BYTE:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          byteval = *(Byte *)get_object_value(operand1) % *(Byte *)get_object_value(operand2);
          vptr = &byteval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BYTE));
          break;
        case CCT_TYPE_NUMBER:
          numval = *(Byte *)get_object_value(operand1) % *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = *(Byte *)get_object_value(operand1) % *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = (Decimal)(*(Byte *)get_object_value(operand1) % (BigNum)(*(Decimal *)get_object_value(operand2)));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (%%)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_NUMBER:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          numval = *(Number *)get_object_value(operand1) % *(Byte *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_NUMBER:
          numval = *(Number *)get_object_value(operand1) % *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = *(Number *)get_object_value(operand1) % *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = (Decimal)(*(Number *)get_object_value(operand1) % (BigNum)(*(Decimal *)get_object_value(operand2)));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (%%)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_BIGNUM:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          bignumval = *(BigNum *)get_object_value(operand1) % *(Byte *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_NUMBER:
          bignumval = *(BigNum *)get_object_value(operand1) % *(Number *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = *(BigNum *)get_object_value(operand1) % *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = (Decimal)(*(BigNum *)get_object_value(operand1) % (BigNum)(*(Decimal *)get_object_value(operand2)));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (%%)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_DECIMAL:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          decimalval = (Decimal)((BigNum)(*(Decimal *)get_object_value(operand1)) % *(Byte *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_NUMBER:
          decimalval = (Decimal)((BigNum)(*(Decimal *)get_object_value(operand1)) % *(Number *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_BIGNUM:
          decimalval = (Decimal)((BigNum)(*(Decimal *)get_object_value(operand1)) % *(BigNum *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = (Decimal)((BigNum)(*(Decimal *)get_object_value(operand1)) % (BigNum)(*(Decimal *)get_object_value(operand2)));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (%%)!\n");
          return RUN_ERROR;
      }
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (%%)!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Exponentiation (**)
RunCode op_pow(Stack* stack)
{
  Object* operand1 = pop(stack); // base
  Object* operand2 = pop(stack); // exponent/power to raise by
  Byte byteval = 0;
  Number numval = 0;
  BigNum bignumval = 0;
  Decimal decimalval = 0.0;
  void* vptr = NULL;

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during POW operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during POW operation.\n");
    return RUN_ERROR;
  }

  if(binary_operand_check(operand1, operand2, "**") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand1->datatype)
  {
    case CCT_TYPE_BYTE:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          byteval = (Byte)(pow(*(Byte *)get_object_value(operand1), *(Byte *)get_object_value(operand2)));
          vptr = &byteval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BYTE));
          break;
        case CCT_TYPE_NUMBER:
          numval = (Number)(pow(*(Byte *)get_object_value(operand1), *(Number *)get_object_value(operand2)));
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = (BigNum)(pow(*(Byte *)get_object_value(operand1), (double)(*(BigNum *)get_object_value(operand2))));
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = pow(*(Byte *)get_object_value(operand1), *(Decimal *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (**)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_NUMBER:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          numval = (Number)(pow(*(Number *)get_object_value(operand1), *(Byte *)get_object_value(operand2)));
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_NUMBER:
          numval = (Number)(pow(*(Number *)get_object_value(operand1), *(Number *)get_object_value(operand2)));
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = (Number)(pow(*(Number *)get_object_value(operand1), (double)(*(BigNum *)get_object_value(operand2))));
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = pow(*(Number *)get_object_value(operand1), *(Decimal *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (**)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_BIGNUM:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          bignumval = (BigNum)(pow((double)(*(BigNum *)get_object_value(operand1)), *(Byte *)get_object_value(operand2)));
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_NUMBER:
          bignumval = (BigNum)(pow((double)(*(BigNum *)get_object_value(operand1)), *(Number *)get_object_value(operand2)));
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = (BigNum)(pow((double)(*(BigNum *)get_object_value(operand1)), (double)(*(BigNum *)get_object_value(operand2))));
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = (Decimal)(pow((double)(*(BigNum *)get_object_value(operand1)), *(Decimal *)get_object_value(operand2)));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (**)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_DECIMAL:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          decimalval = pow(*(Decimal *)get_object_value(operand1), *(Byte *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_NUMBER:
          decimalval = pow(*(Decimal *)get_object_value(operand1), *(Number *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_BIGNUM:
          decimalval = pow(*(Decimal *)get_object_value(operand1), (double)(*(BigNum *)get_object_value(operand2)));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = pow(*(Decimal *)get_object_value(operand1), *(Decimal *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (**)!\n");
          return RUN_ERROR;
      }
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (**)!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Bitwise and (&)
RunCode op_bnd(Stack* stack)
{
  Object* operand1 = pop(stack);
  Object* operand2 = pop(stack);
  Byte byteval = 0;
  Number numval = 0;
  BigNum bignumval = 0;
  Decimal decimalval = 0.0;
  void* vptr = NULL;

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during BND operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during BND operation.\n");
    return RUN_ERROR;
  }

  if(binary_operand_check(operand1, operand2, "&") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand1->datatype)
  {
    case CCT_TYPE_BYTE:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          byteval = *(Byte *)get_object_value(operand1) & *(Byte *)get_object_value(operand2);
          vptr = &byteval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BYTE));
          break;
        case CCT_TYPE_NUMBER:
          numval = *(Byte *)get_object_value(operand1) & *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = *(Byte *)get_object_value(operand1) & *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = *(Byte *)get_object_value(operand1) & (Number)(*(Decimal *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (&)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_NUMBER:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          numval = *(Number *)get_object_value(operand1) & *(Byte *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_NUMBER:
          numval = *(Number *)get_object_value(operand1) & *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = *(Number *)get_object_value(operand1) & *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = *(Number *)get_object_value(operand1) & (Number)(*(Decimal *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (&)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_BIGNUM:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          bignumval = *(BigNum *)get_object_value(operand1) & *(Byte *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_NUMBER:
          bignumval = *(BigNum *)get_object_value(operand1) & *(Number *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = *(BigNum *)get_object_value(operand1) & *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = (Decimal)(*(BigNum *)get_object_value(operand1) & (Number)(*(Decimal *)get_object_value(operand2)));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (&)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_DECIMAL:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          decimalval = (Number)(*(Decimal *)get_object_value(operand1)) & *(Byte *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_NUMBER:
          decimalval = (Number)(*(Decimal *)get_object_value(operand1)) & *(Number *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_BIGNUM:
          decimalval = (Decimal)((Number)(*(Decimal *)get_object_value(operand1)) & *(BigNum *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = (Number)(*(Decimal *)get_object_value(operand1)) & (Number)(*(Decimal *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (&)!\n");
          return RUN_ERROR;
      }
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (&)!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Bitwise or (|)
RunCode op_bor(Stack* stack)
{
  Object* operand1 = pop(stack);
  Object* operand2 = pop(stack);
  Byte byteval = 0;
  Number numval = 0;
  BigNum bignumval = 0;
  Decimal decimalval = 0.0;
  void* vptr = NULL;

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during BOR operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during BOR operation.\n");
    return RUN_ERROR;
  }

  if(binary_operand_check(operand1, operand2, "|") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand1->datatype)
  {
    case CCT_TYPE_BYTE:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          byteval = *(Byte *)get_object_value(operand1) | *(Byte *)get_object_value(operand2);
          vptr = &byteval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BYTE));
          break;
        case CCT_TYPE_NUMBER:
          numval = *(Byte *)get_object_value(operand1) | *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = *(Byte *)get_object_value(operand1) | *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = *(Byte *)get_object_value(operand1) | (Number)(*(Decimal *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (|)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_NUMBER:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          numval = *(Number *)get_object_value(operand1) | *(Byte *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_NUMBER:
          numval = *(Number *)get_object_value(operand1) | *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = *(Number *)get_object_value(operand1) | *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = *(Number *)get_object_value(operand1) | (Number)(*(Decimal *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (|)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_BIGNUM:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          bignumval = *(BigNum *)get_object_value(operand1) | *(Byte *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_NUMBER:
          bignumval = *(BigNum *)get_object_value(operand1) | *(Number *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = *(BigNum *)get_object_value(operand1) | *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = (Decimal)(*(BigNum *)get_object_value(operand1) | (Number)(*(Decimal *)get_object_value(operand2)));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (|)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_DECIMAL:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          decimalval = (Number)(*(Decimal *)get_object_value(operand1)) | *(Byte *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_NUMBER:
          decimalval = (Number)(*(Decimal *)get_object_value(operand1)) | *(Number *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_BIGNUM:
          decimalval = (Decimal)((Number)(*(Decimal *)get_object_value(operand1)) | *(BigNum *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = (Number)(*(Decimal *)get_object_value(operand1)) | (Number)(*(Decimal *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (|)!\n");
          return RUN_ERROR;
      }
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (|)!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Bitwise xor (^)
RunCode op_xor(Stack* stack)
{
  Object* operand1 = pop(stack);
  Object* operand2 = pop(stack);
  Byte byteval = 0;
  Number numval = 0;
  BigNum bignumval = 0;
  Decimal decimalval = 0.0;
  void* vptr = NULL;

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during XOR operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during XOR operation.\n");
    return RUN_ERROR;
  }

  if(binary_operand_check(operand1, operand2, "^") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand1->datatype)
  {
    case CCT_TYPE_BYTE:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          byteval = *(Byte *)get_object_value(operand1) ^ *(Byte *)get_object_value(operand2);
          vptr = &byteval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BYTE));
          break;
        case CCT_TYPE_NUMBER:
          numval = *(Byte *)get_object_value(operand1) ^ *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = *(Byte *)get_object_value(operand1) ^ *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = *(Byte *)get_object_value(operand1) ^ (Number)(*(Decimal *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (^)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_NUMBER:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          numval = *(Number *)get_object_value(operand1) ^ *(Byte *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_NUMBER:
          numval = *(Number *)get_object_value(operand1) ^ *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = *(Number *)get_object_value(operand1) ^ *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = *(Number *)get_object_value(operand1) ^ (Number)(*(Decimal *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (^)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_BIGNUM:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          bignumval = *(BigNum *)get_object_value(operand1) ^ *(Byte *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_NUMBER:
          bignumval = *(BigNum *)get_object_value(operand1) ^ *(Number *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_BIGNUM:
          bignumval = *(BigNum *)get_object_value(operand1) ^ *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = (Decimal)(*(BigNum *)get_object_value(operand1) ^ (Number)(*(Decimal *)get_object_value(operand2)));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (^)!\n");
          return RUN_ERROR;
      }
      break;
    case CCT_TYPE_DECIMAL:
      switch(operand2->datatype)
      {
        case CCT_TYPE_BYTE:
          decimalval = (Number)(*(Decimal *)get_object_value(operand1)) ^ *(Byte *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_NUMBER:
          decimalval = (Number)(*(Decimal *)get_object_value(operand1)) ^ *(Number *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_BIGNUM:
          decimalval = (Decimal)((Number)(*(Decimal *)get_object_value(operand1)) ^ *(BigNum *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        case CCT_TYPE_DECIMAL:
          decimalval = (Number)(*(Decimal *)get_object_value(operand1)) ^ (Number)(*(Decimal *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (^)!\n");
          return RUN_ERROR;
      }
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (^)!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Bitwise not/ones' complement (~)
RunCode op_bnt(Stack* stack)
{
  Object* operand = pop(stack);

  Byte byteval = 0;
  Number numval = 0;
  BigNum bignumval = 0;
  Decimal decimalval = 0.0;
  void* vptr = NULL;

  if(operand == NULL)
  {
    fprintf(stderr, "Operand is NULL during BNT operation.\n");
    return RUN_ERROR;
  }

  if(unary_operand_check(operand, "~") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand->datatype)
  {
    case CCT_TYPE_BYTE:
      byteval = *(Byte *)get_object_value(operand);
      byteval = ~byteval;
      vptr = &byteval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_BYTE));
      break;
    case CCT_TYPE_NUMBER:
      numval = *(Number *)get_object_value(operand);
      numval = ~numval;
      vptr = &numval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
      break;
    case CCT_TYPE_BIGNUM:
      bignumval = *(BigNum *)get_object_value(operand);
      bignumval = ~bignumval;
      vptr = &bignumval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
      break;
    case CCT_TYPE_DECIMAL:
      decimalval = *(Decimal *)get_object_value(operand);
      decimalval = ~(Number)decimalval;
      vptr = &decimalval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (~)!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Bit shift left (<<)
RunCode op_shl(Stack* stack)
{
  Object* operand1 = pop(stack); // number to shift
  Object* operand2 = pop(stack); // positions to shift
  Byte byteval = 0;
  Number numval = 0;
  BigNum bignumval = 0;
  Decimal decimalval = 0.0;
  void* vptr = NULL;

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during SHL operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during SHL operation.\n");
    return RUN_ERROR;
  }

  if(binary_operand_check(operand1, operand2, "<<") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand1->datatype)
  {
    case CCT_TYPE_BYTE:
      byteval = *(Byte *)get_object_value(operand1);
      byteval = byteval << *(Number *)get_object_value(operand2);
      vptr = &byteval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_BYTE));
      break;
    case CCT_TYPE_NUMBER:
      numval = *(Number *)get_object_value(operand1);
      numval = numval << *(Number *)get_object_value(operand2);
      vptr = &numval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
      break;
    case CCT_TYPE_BIGNUM:
      bignumval = *(BigNum *)get_object_value(operand1);
      bignumval = bignumval << *(Number *)get_object_value(operand2);
      vptr = &bignumval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
      break;
    case CCT_TYPE_DECIMAL:
      decimalval = *(Decimal *)get_object_value(operand1);
      decimalval = (Number)decimalval << *(Number *)get_object_value(operand2);
      vptr = &decimalval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (<<)!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Bit shift right (>>)
RunCode op_shr(Stack* stack)
{
  Object* operand1 = pop(stack); // number to shift
  Object* operand2 = pop(stack); // positions to shift
  Byte byteval = 0;
  Number numval = 0;
  BigNum bignumval = 0;
  Decimal decimalval = 0.0;
  void* vptr = NULL;

  if(operand1 == NULL)
  {
    fprintf(stderr, "Operand 1 is NULL during SHR operation.\n");
    return RUN_ERROR;
  }

  if(operand2 == NULL)
  {
    fprintf(stderr, "Operand 2 is NULL during SHR operation.\n");
    return RUN_ERROR;
  }

  if(binary_operand_check(operand1, operand2, ">>") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand1->datatype)
  {
    case CCT_TYPE_BYTE:
      byteval = *(Byte *)get_object_value(operand1);
      byteval = byteval >> *(Number *)get_object_value(operand2);
      vptr = &byteval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_BYTE));
      break;
    case CCT_TYPE_NUMBER:
      numval = *(Number *)get_object_value(operand1);
      numval = numval >> *(Number *)get_object_value(operand2);
      vptr = &numval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_NUMBER));
      break;
    case CCT_TYPE_BIGNUM:
      bignumval = *(BigNum *)get_object_value(operand1);
      bignumval = bignumval >> *(Number *)get_object_value(operand2);
      vptr = &bignumval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
      break;
    case CCT_TYPE_DECIMAL:
      decimalval = *(Decimal *)get_object_value(operand1);
      decimalval = (Number)decimalval >> *(Number *)get_object_value(operand2);
      vptr = &decimalval;
      push(stack, new_object_by_type(vptr, CCT_TYPE_DECIMAL));
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (>>)!\n");
      return RUN_ERROR;
  }
  return RUN_SUCCESS;
}
