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

#include <math.h>   // pow()
#include <stdio.h>  // fprintf(), stderr
#include <string.h> // strcat()
#include "memory.h"
#include "vm/vm.h"

// Validates unary operand
RunCode unary_check(Object* operand, char* operator)
{
  if(operand->datatype == NIL)
  {
    fprintf(stderr, "Invalid operation (%s) for object of type \"null\"!\n", operator);
    return RUN_ERROR;
  }
  if(operand->datatype == BOOL)
  {
    fprintf(stderr, "Invalid operation (%s) for object of type \"boolean\"!\n", operator);
    return RUN_ERROR;
  }
  if(operand->datatype == STRING)
  {
    fprintf(stderr, "Invalid operation (%s) for object of type \"string\"!\n", operator);
    return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Validates binary mathematical operands
RunCode binary_math_check(Object* operand1, Object* operand2, char* operator)
{
  if(operand1->datatype == NIL || operand2->datatype == NIL)
  {
    fprintf(stderr, "Invalid operation (%s) for object of type \"null\"!\n", operator);
    return RUN_ERROR;
  }
  if(operand1->datatype == BOOL || operand2->datatype == BOOL)
  {
    fprintf(stderr, "Invalid operation (%s) for object of type \"boolean\"!\n", operator);
    return RUN_ERROR;
  }
  if((operand1->datatype == STRING && operand2->datatype != STRING) || (operand1->datatype != STRING && operand2->datatype == STRING))
  {
    fprintf(stderr, "Invalid binary operation (%s) for single operand of type \"string\"!\n", operator);
    return RUN_ERROR;
  }
  return RUN_SUCCESS;
}

// Pop
RunCode op_pop(Stack* stack, Object* object)
{
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

// And
RunCode op_and(Stack* stack)
{
  Object* operand2 = pop(stack);
  Object* operand1 = pop(stack);
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

  if(operand1->datatype != BOOL && operand2->datatype != BOOL)
  {
    fprintf(stderr, "Invalid operation (&&) for non-bool object!\n");
    return RUN_ERROR;
  }

  result = *(Bool *)get_object_value(operand1) && *(Bool *)get_object_value(operand2);
  vptr = &result;
  push(stack, new_object_by_type(vptr, BOOL));

  return RUN_SUCCESS;
}

// Not/negation
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

  if(operand->datatype != BOOL)
  {
    fprintf(stderr, "Invalid operation (!) for non-bool object!\n");
    return RUN_ERROR;
  }

  result = *(Bool *)get_object_value(operand);
  result = !result;
  vptr = &result;
  push(stack, new_object_by_type(vptr, BOOL));

  return RUN_SUCCESS;
}

// Or
RunCode op_or(Stack* stack)
{
  Object* operand2 = pop(stack);
  Object* operand1 = pop(stack);
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

  if(operand1->datatype != BOOL && operand2->datatype != BOOL)
  {
    fprintf(stderr, "Invalid operation (||) for non-bool object!\n");
    return RUN_ERROR;
  }

  result = *(Bool *)get_object_value(operand1) || *(Bool *)get_object_value(operand2);
  vptr = &result;
  push(stack, new_object_by_type(vptr, BOOL));

  return RUN_SUCCESS;
}

// Decrement
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

  if(unary_check(operand, "--") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand->datatype)
  {
    case BYTE:
      byteval = *(Byte *)get_object_value(operand);
      byteval--;
      vptr = &byteval;
      push(stack, new_object_by_type(vptr, BYTE));
      break;
    case NUMBER:
      numval = *(Byte *)get_object_value(operand);
      numval--;
      vptr = &numval;
      push(stack, new_object_by_type(vptr, NUMBER));
      break;
    case BIGNUM:
      bignumval = *(Byte *)get_object_value(operand);
      bignumval--;
      vptr = &bignumval;
      push(stack, new_object_by_type(vptr, BIGNUM));
      break;
    case DECIMAL:
      decimalval = *(Byte *)get_object_value(operand);
      decimalval--;
      vptr = &decimalval;
      push(stack, new_object_by_type(vptr, DECIMAL));
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (--)!\n");
      return RUN_ERROR;
      break;
  }
  return RUN_SUCCESS;
}

// Increment
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

  if(unary_check(operand, "++") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand->datatype)
  {
    case BYTE:
      byteval = *(Byte *)get_object_value(operand);
      byteval++;
      vptr = &byteval;
      push(stack, new_object_by_type(vptr, BYTE));
      break;
    case NUMBER:
      numval = *(Byte *)get_object_value(operand);
      numval++;
      vptr = &numval;
      push(stack, new_object_by_type(vptr, NUMBER));
      break;
    case BIGNUM:
      bignumval = *(Byte *)get_object_value(operand);
      bignumval++;
      vptr = &bignumval;
      push(stack, new_object_by_type(vptr, BIGNUM));
      break;
    case DECIMAL:
      decimalval = *(Byte *)get_object_value(operand);
      decimalval++;
      vptr = &decimalval;
      push(stack, new_object_by_type(vptr, DECIMAL));
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (++)!\n");
      return RUN_ERROR;
      break;
  }
  return RUN_SUCCESS;
}

// Addition
RunCode op_add(Stack* stack)
{
  Object* operand2 = pop(stack);
  Object* operand1 = pop(stack);
  Byte byteval = 0;
  Number numval = 0;
  BigNum bignumval = 0;
  Decimal decimalval = 0.0;
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

  if(binary_math_check(operand1, operand2, "+") == RUN_ERROR)
    return RUN_ERROR;

  if(operand1->datatype == STRING && operand2->datatype == STRING) // string concatenation
    push(stack, new_object(strcat(operand1->value.strobj.strval, operand2->value.strobj.strval)));
  else
  {
    switch(operand1->datatype)
    {
      case BYTE:
        switch(operand2->datatype)
        {
          case BYTE:
            byteval = *(Byte *)get_object_value(operand1) + *(Byte *)get_object_value(operand2);
            vptr = &byteval;
            push(stack, new_object_by_type(vptr, BYTE));
            break;
          case NUMBER:
            numval = *(Byte *)get_object_value(operand1) + *(Number *)get_object_value(operand2);
            vptr = &numval;
            push(stack, new_object_by_type(vptr, NUMBER));
            break;
          case BIGNUM:
            bignumval = *(Byte *)get_object_value(operand1) + *(BigNum *)get_object_value(operand2);
            vptr = &bignumval;
            push(stack, new_object_by_type(vptr, BIGNUM));
            break;
          case DECIMAL:
            decimalval = *(Byte *)get_object_value(operand1) + *(Decimal *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, DECIMAL));
            break;
          default:
            fprintf(stderr, "Invalid operand type encountered during operation (+)!\n");
            return RUN_ERROR;
            break;
        }
        break;
      case NUMBER:
        switch(operand2->datatype)
        {
          case BYTE:
            numval = *(Number *)get_object_value(operand1) + *(Byte *)get_object_value(operand2);
            vptr = &numval;
            push(stack, new_object_by_type(vptr, NUMBER));
            break;
          case NUMBER:
            numval = *(Number *)get_object_value(operand1) + *(Number *)get_object_value(operand2);
            vptr = &numval;
            push(stack, new_object_by_type(vptr, NUMBER));
            break;
          case BIGNUM:
            bignumval = *(Number *)get_object_value(operand1) + *(BigNum *)get_object_value(operand2);
            vptr = &bignumval;
            push(stack, new_object_by_type(vptr, BIGNUM));
            break;
          case DECIMAL:
            decimalval = *(Number *)get_object_value(operand1) + *(Decimal *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, DECIMAL));
            break;
          default:
            fprintf(stderr, "Invalid operand type encountered during operation (+)!\n");
            return RUN_ERROR;
            break;
        }
        break;
      case BIGNUM:
        switch(operand2->datatype)
        {
          case BYTE:
            bignumval = *(BigNum *)get_object_value(operand1) + *(Byte *)get_object_value(operand2);
            vptr = &bignumval;
            push(stack, new_object_by_type(vptr, BIGNUM));
            break;
          case NUMBER:
            bignumval = *(BigNum *)get_object_value(operand1) + *(Number *)get_object_value(operand2);
            vptr = &bignumval;
            push(stack, new_object_by_type(vptr, BIGNUM));
            break;
          case BIGNUM:
            bignumval = *(BigNum *)get_object_value(operand1) + *(BigNum *)get_object_value(operand2);
            vptr = &bignumval;
            push(stack, new_object_by_type(vptr, BIGNUM));
            break;
          case DECIMAL:
            decimalval = *(BigNum *)get_object_value(operand1) + *(Decimal *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, DECIMAL));
            break;
          default:
            fprintf(stderr, "Invalid operand type encountered during operation (+)!\n");
            return RUN_ERROR;
            break;
        }
        break;
      case DECIMAL:
        switch(operand2->datatype)
        {
          case BYTE:
            decimalval = *(Decimal *)get_object_value(operand1) + *(Byte *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, DECIMAL));
            break;
          case NUMBER:
            decimalval = *(Decimal *)get_object_value(operand1) + *(Number *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, DECIMAL));
            break;
          case BIGNUM:
            decimalval = *(Decimal *)get_object_value(operand1) + *(BigNum *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, DECIMAL));
            break;
          case DECIMAL:
            decimalval = *(Decimal *)get_object_value(operand1) + *(Decimal *)get_object_value(operand2);
            vptr = &decimalval;
            push(stack, new_object_by_type(vptr, DECIMAL));
            break;
          default:
            fprintf(stderr, "Invalid operand type encountered during operation (+)!\n");
            return RUN_ERROR;
            break;
        }
        break;
      default:
        fprintf(stderr, "Invalid operand type encountered during operation (+)!\n");
        return RUN_ERROR;
        break;
    }
  }
  return RUN_SUCCESS;
}

// Subtraction
RunCode op_sub(Stack* stack)
{
  Object* operand2 = pop(stack);
  Object* operand1 = pop(stack);
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

  if(binary_math_check(operand1, operand2, "-") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand1->datatype)
  {
    case BYTE:
      switch(operand2->datatype)
      {
        case BYTE:
          byteval = *(Byte *)get_object_value(operand1) - *(Byte *)get_object_value(operand2);
          vptr = &byteval;
          push(stack, new_object_by_type(vptr, BYTE));
          break;
        case NUMBER:
          numval = *(Byte *)get_object_value(operand1) - *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, NUMBER));
          break;
        case BIGNUM:
          bignumval = *(Byte *)get_object_value(operand1) - *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case DECIMAL:
          decimalval = *(Byte *)get_object_value(operand1) - *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (-)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    case NUMBER:
      switch(operand2->datatype)
      {
        case BYTE:
          numval = *(Number *)get_object_value(operand1) - *(Byte *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, NUMBER));
          break;
        case NUMBER:
          numval = *(Number *)get_object_value(operand1) - *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, NUMBER));
          break;
        case BIGNUM:
          bignumval = *(Number *)get_object_value(operand1) - *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case DECIMAL:
          decimalval = *(Number *)get_object_value(operand1) - *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (-)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    case BIGNUM:
      switch(operand2->datatype)
      {
        case BYTE:
          bignumval = *(BigNum *)get_object_value(operand1) - *(Byte *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case NUMBER:
          bignumval = *(BigNum *)get_object_value(operand1) - *(Number *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case BIGNUM:
          bignumval = *(BigNum *)get_object_value(operand1) - *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case DECIMAL:
          decimalval = *(BigNum *)get_object_value(operand1) - *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (-)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    case DECIMAL:
      switch(operand2->datatype)
      {
        case BYTE:
          decimalval = *(Decimal *)get_object_value(operand1) - *(Byte *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        case NUMBER:
          decimalval = *(Decimal *)get_object_value(operand1) - *(Number *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        case BIGNUM:
          decimalval = *(Decimal *)get_object_value(operand1) - *(BigNum *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        case DECIMAL:
          decimalval = *(Decimal *)get_object_value(operand1) - *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (-)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (-)!\n");
      return RUN_ERROR;
      break;
  }
  return RUN_SUCCESS;
}

// Division
RunCode op_div(Stack* stack)
{
  Object* operand2 = pop(stack);
  Object* operand1 = pop(stack);
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
    case BYTE:
      if(*(Byte *)get_object_value(operand2) == 0)
        div_by_zero = true;
      break;
    case NUMBER:
      if(*(Number *)get_object_value(operand2) == 0)
        div_by_zero = true;
      break;
    case BIGNUM:
      if(*(BigNum *)get_object_value(operand2) == 0)
        div_by_zero = true;
      break;
    case DECIMAL:
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

  if(binary_math_check(operand1, operand2, "/") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand1->datatype)
  {
    case BYTE:
      switch(operand2->datatype)
      {
        case BYTE:
          byteval = *(Byte *)get_object_value(operand1) / *(Byte *)get_object_value(operand2);
          vptr = &byteval;
          push(stack, new_object_by_type(vptr, BYTE));
          break;
        case NUMBER:
          numval = *(Byte *)get_object_value(operand1) / *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, NUMBER));
          break;
        case BIGNUM:
          bignumval = *(Byte *)get_object_value(operand1) / *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case DECIMAL:
          decimalval = *(Byte *)get_object_value(operand1) / *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (/)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    case NUMBER:
      switch(operand2->datatype)
      {
        case BYTE:
          numval = *(Number *)get_object_value(operand1) / *(Byte *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, NUMBER));
          break;
        case NUMBER:
          numval = *(Number *)get_object_value(operand1) / *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, NUMBER));
          break;
        case BIGNUM:
          bignumval = *(Number *)get_object_value(operand1) / *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case DECIMAL:
          decimalval = *(Number *)get_object_value(operand1) / *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (/)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    case BIGNUM:
      switch(operand2->datatype)
      {
        case BYTE:
          bignumval = *(BigNum *)get_object_value(operand1) / *(Byte *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case NUMBER:
          bignumval = *(BigNum *)get_object_value(operand1) / *(Number *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case BIGNUM:
          bignumval = *(BigNum *)get_object_value(operand1) / *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case DECIMAL:
          decimalval = *(BigNum *)get_object_value(operand1) / *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (/)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    case DECIMAL:
      switch(operand2->datatype)
      {
        case BYTE:
          decimalval = *(Decimal *)get_object_value(operand1) / *(Byte *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        case NUMBER:
          decimalval = *(Decimal *)get_object_value(operand1) / *(Number *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        case BIGNUM:
          decimalval = *(Decimal *)get_object_value(operand1) / *(BigNum *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        case DECIMAL:
          decimalval = *(Decimal *)get_object_value(operand1) / *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (/)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (/)!\n");
      return RUN_ERROR;
      break;
  }
  return RUN_SUCCESS;
}

// Multiplication
RunCode op_mul(Stack* stack)
{
  Object* operand2 = pop(stack);
  Object* operand1 = pop(stack);
  Byte byteval = 0;
  Number numval = 0;
  BigNum bignumval = 0;
  Decimal decimalval = 0.0;
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

  if(binary_math_check(operand1, operand2, "*") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand1->datatype)
  {
    case BYTE:
      switch(operand2->datatype)
      {
        case BYTE:
          byteval = *(Byte *)get_object_value(operand1) * *(Byte *)get_object_value(operand2);
          vptr = &byteval;
          push(stack, new_object_by_type(vptr, BYTE));
          break;
        case NUMBER:
          numval = *(Byte *)get_object_value(operand1) * *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, NUMBER));
          break;
        case BIGNUM:
          bignumval = *(Byte *)get_object_value(operand1) * *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case DECIMAL:
          decimalval = *(Byte *)get_object_value(operand1) * *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (*)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    case NUMBER:
      switch(operand2->datatype)
      {
        case BYTE:
          numval = *(Number *)get_object_value(operand1) * *(Byte *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, NUMBER));
          break;
        case NUMBER:
          numval = *(Number *)get_object_value(operand1) * *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, NUMBER));
          break;
        case BIGNUM:
          bignumval = *(Number *)get_object_value(operand1) * *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case DECIMAL:
          decimalval = *(Number *)get_object_value(operand1) * *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (*)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    case BIGNUM:
      switch(operand2->datatype)
      {
        case BYTE:
          bignumval = *(BigNum *)get_object_value(operand1) * *(Byte *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case NUMBER:
          bignumval = *(BigNum *)get_object_value(operand1) * *(Number *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case BIGNUM:
          bignumval = *(BigNum *)get_object_value(operand1) * *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case DECIMAL:
          decimalval = *(BigNum *)get_object_value(operand1) * *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (*)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    case DECIMAL:
      switch(operand2->datatype)
      {
        case BYTE:
          decimalval = *(Decimal *)get_object_value(operand1) * *(Byte *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        case NUMBER:
          decimalval = *(Decimal *)get_object_value(operand1) * *(Number *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        case BIGNUM:
          decimalval = *(Decimal *)get_object_value(operand1) * *(BigNum *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        case DECIMAL:
          decimalval = *(Decimal *)get_object_value(operand1) * *(Decimal *)get_object_value(operand2);
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (*)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (*)!\n");
      return RUN_ERROR;
      break;
  }
  return RUN_SUCCESS;
}

// Modulo
// Note: Modulo operates on integers. Decimal numbers are truncated.
RunCode op_mod(Stack* stack)
{
  Object* operand2 = pop(stack);
  Object* operand1 = pop(stack);
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

  if(binary_math_check(operand1, operand2, "%") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand1->datatype)
  {
    case BYTE:
      switch(operand2->datatype)
      {
        case BYTE:
          byteval = *(Byte *)get_object_value(operand1) % *(Byte *)get_object_value(operand2);
          vptr = &byteval;
          push(stack, new_object_by_type(vptr, BYTE));
          break;
        case NUMBER:
          numval = *(Byte *)get_object_value(operand1) % *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, NUMBER));
          break;
        case BIGNUM:
          bignumval = *(Byte *)get_object_value(operand1) % *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case DECIMAL:
          decimalval = (Decimal)(*(Byte *)get_object_value(operand1) % (BigNum)(*(Decimal *)get_object_value(operand2)));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (%%)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    case NUMBER:
      switch(operand2->datatype)
      {
        case BYTE:
          numval = *(Number *)get_object_value(operand1) % *(Byte *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, NUMBER));
          break;
        case NUMBER:
          numval = *(Number *)get_object_value(operand1) % *(Number *)get_object_value(operand2);
          vptr = &numval;
          push(stack, new_object_by_type(vptr, NUMBER));
          break;
        case BIGNUM:
          bignumval = *(Number *)get_object_value(operand1) % *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case DECIMAL:
          decimalval = (Decimal)(*(Number *)get_object_value(operand1) % (BigNum)(*(Decimal *)get_object_value(operand2)));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (%%)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    case BIGNUM:
      switch(operand2->datatype)
      {
        case BYTE:
          bignumval = *(BigNum *)get_object_value(operand1) % *(Byte *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case NUMBER:
          bignumval = *(BigNum *)get_object_value(operand1) % *(Number *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case BIGNUM:
          bignumval = *(BigNum *)get_object_value(operand1) % *(BigNum *)get_object_value(operand2);
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case DECIMAL:
          decimalval = (Decimal)(*(BigNum *)get_object_value(operand1) % (BigNum)(*(Decimal *)get_object_value(operand2)));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (%%)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    case DECIMAL:
      switch(operand2->datatype)
      {
        case BYTE:
          decimalval = (Decimal)((BigNum)(*(Decimal *)get_object_value(operand1)) % *(Byte *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        case NUMBER:
          decimalval = (Decimal)((BigNum)(*(Decimal *)get_object_value(operand1)) % *(Number *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        case BIGNUM:
          decimalval = (Decimal)((BigNum)(*(Decimal *)get_object_value(operand1)) % *(BigNum *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        case DECIMAL:
          decimalval = (Decimal)((BigNum)(*(Decimal *)get_object_value(operand1)) % (BigNum)(*(Decimal *)get_object_value(operand2)));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (%%)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (%%)!\n");
      return RUN_ERROR;
      break;
  }
  return RUN_SUCCESS;
}

// Exponentiation
RunCode op_pow(Stack* stack)
{
  Object* operand2 = pop(stack);
  Object* operand1 = pop(stack);
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

  if(binary_math_check(operand1, operand2, "**") == RUN_ERROR)
    return RUN_ERROR;

  switch(operand1->datatype)
  {
    case BYTE:
      switch(operand2->datatype)
      {
        case BYTE:
          byteval = (Byte)(pow(*(Byte *)get_object_value(operand1), *(Byte *)get_object_value(operand2)));
          vptr = &byteval;
          push(stack, new_object_by_type(vptr, BYTE));
          break;
        case NUMBER:
          numval = (Number)(pow(*(Byte *)get_object_value(operand1), *(Number *)get_object_value(operand2)));
          vptr = &numval;
          push(stack, new_object_by_type(vptr, NUMBER));
          break;
        case BIGNUM:
          bignumval = (BigNum)(pow(*(Byte *)get_object_value(operand1), (double)(*(BigNum *)get_object_value(operand2))));
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case DECIMAL:
          decimalval = pow(*(Byte *)get_object_value(operand1), *(Decimal *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (**)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    case NUMBER:
      switch(operand2->datatype)
      {
        case BYTE:
          numval = (Number)(pow(*(Number *)get_object_value(operand1), *(Byte *)get_object_value(operand2)));
          vptr = &numval;
          push(stack, new_object_by_type(vptr, NUMBER));
          break;
        case NUMBER:
          numval = (Number)(pow(*(Number *)get_object_value(operand1), *(Number *)get_object_value(operand2)));
          vptr = &numval;
          push(stack, new_object_by_type(vptr, NUMBER));
          break;
        case BIGNUM:
          bignumval = (Number)(pow(*(Number *)get_object_value(operand1), (double)(*(BigNum *)get_object_value(operand2))));
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case DECIMAL:
          decimalval = pow(*(Number *)get_object_value(operand1), *(Decimal *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (**)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    case BIGNUM:
      switch(operand2->datatype)
      {
        case BYTE:
          bignumval = (BigNum)(pow((double)(*(BigNum *)get_object_value(operand1)), *(Byte *)get_object_value(operand2)));
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case NUMBER:
          bignumval = (BigNum)(pow((double)(*(BigNum *)get_object_value(operand1)), *(Number *)get_object_value(operand2)));
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case BIGNUM:
          bignumval = (BigNum)(pow((double)(*(BigNum *)get_object_value(operand1)), (double)(*(BigNum *)get_object_value(operand2))));
          vptr = &bignumval;
          push(stack, new_object_by_type(vptr, BIGNUM));
          break;
        case DECIMAL:
          decimalval = (Decimal)(pow((double)(*(BigNum *)get_object_value(operand1)), *(Decimal *)get_object_value(operand2)));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (**)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    case DECIMAL:
      switch(operand2->datatype)
      {
        case BYTE:
          decimalval = pow(*(Decimal *)get_object_value(operand1), *(Byte *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        case NUMBER:
          decimalval = pow(*(Decimal *)get_object_value(operand1), *(Number *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        case BIGNUM:
          decimalval = pow(*(Decimal *)get_object_value(operand1), (double)(*(BigNum *)get_object_value(operand2)));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        case DECIMAL:
          decimalval = pow(*(Decimal *)get_object_value(operand1), *(Decimal *)get_object_value(operand2));
          vptr = &decimalval;
          push(stack, new_object_by_type(vptr, DECIMAL));
          break;
        default:
          fprintf(stderr, "Invalid operand type encountered during operation (**)!\n");
          return RUN_ERROR;
          break;
      }
      break;
    default:
      fprintf(stderr, "Invalid operand type encountered during operation (**)!\n");
      return RUN_ERROR;
      break;
  }
  return RUN_SUCCESS;
}
