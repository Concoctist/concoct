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

#include <stdio.h>  // fprintf(), stderr
#include <string.h> // strcat()
#include "memory.h"
#include "vm/vm.h"

// Validates binary mathematical operands
RunCode binary_math_check(Object* operand1, Object* operand2, char operator)
{
  if(operand1->datatype == NIL || operand2->datatype == NIL)
  {
    fprintf(stderr, "Invalid operation (%c) for object of type \"null\"!\n", operator);
    return RUN_ERROR;
  }
  if(operand1->datatype == BOOL || operand2->datatype == BOOL)
  {
    fprintf(stderr, "Invalid operation (%c) for object of type \"boolean\"!\n", operator);
    return RUN_ERROR;
  }
  if((operand1->datatype == STRING && operand2->datatype != STRING) || (operand1->datatype != STRING && operand2->datatype == STRING))
  {
    fprintf(stderr, "Invalid binary operation (%c) for single operand of type \"string\"!\n", operator);
    return RUN_ERROR;
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
  if(binary_math_check(operand1, operand2, '+') == RUN_ERROR)
    return RUN_ERROR;

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

  if(operand1->datatype == STRING && operand2->datatype == STRING) // string concatenation
    push(stack, new_object(strcat(operand1->value.strobj.strval, operand2->value.strobj.strval)));
  else
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
  return RUN_SUCCESS;
}

/*
// Subtraction
RunCode op_sub(Stack* stack)
{
  Object* operand2 = pop(stack);
  Object* operand1 = pop(stack);
  //push(stack, pop(stack) - pop(stack));
  return RUN_SUCCESS;
}

// Division
RunCode op_div(Stack* stack)
{
  // Note: Handle divide by zero!
  Object* operand2 = pop(stack);
  Object* operand1 = pop(stack);
  //push(stack, pop(stack) / pop(stack));
  return RUN_SUCCESS;
}

// Multiplication
RunCode op_mul(Stack* stack)
{
  Object* operand2 = pop(stack);
  Object* operand1 = pop(stack);
  //push(stack, pop(stack) * pop(stack));
  return RUN_SUCCESS;
}

// Modulo
RunCode op_mod(Stack* stack)
{
  Object* operand2 = pop(stack);
  Object* operand1 = pop(stack);
  //push(stack, pop(stack) % pop(stack));
  return RUN_SUCCESS;
}
*/
