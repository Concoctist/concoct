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

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "vm/stack.h"
#include "vm/vm.h"

#define OP_NOOP (void)0

RunCode unary_operand_check(Object* operand, char* operator);
RunCode binary_operand_check(Object* operand1, Object* operand2, char operator);
RunCode binary_operand_check_str(Object* operand1, Object* operand2, char operator);
RunCode op_pop(Stack* stack, Object* object);
RunCode op_psh(Stack* stack, char* value);
RunCode op_fls(Stack* stack);
RunCode op_tru(Stack* stack);
RunCode op_and(Stack* stack);
RunCode op_not(Stack* stack);
RunCode op_or(Stack* stack);
RunCode op_neg(Stack* stack);
RunCode op_pos(Stack* stack);
RunCode op_dec(Stack* stack);
RunCode op_inc(Stack* stack);
RunCode op_add(Stack* stack);
RunCode op_sub(Stack* stack);
RunCode op_div(Stack* stack);
RunCode op_mul(Stack* stack);
RunCode op_mod(Stack* stack);
RunCode op_pow(Stack* stack);
RunCode op_bnd(Stack* stack);
RunCode op_bor(Stack* stack);
RunCode op_xor(Stack* stack);
RunCode op_bnt(Stack* stack);
RunCode op_shl(Stack* stack);
RunCode op_shr(Stack* stack);

#endif // INSTRUCTIONS_H
