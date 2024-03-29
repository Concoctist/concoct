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

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "hash_map.h"
#include "stack.h"
#include "vm/vm.h"

#define OP_NOOP (void)0

RunCode unary_operand_check(const Object* operand, char* operator);
RunCode binary_operand_check(const Object* operand1, const Object* operand2, char* operator);
RunCode binary_operand_check_str(const Object* operand1, const Object* operand2, char* operator);
RunCode op_clr(Object** rp);
RunCode op_cls(Stack* stack);
RunCode op_lod(Object** rp, Stack* stack, Byte dst_reg);
RunCode op_mov(Object** rp, Object* object, Byte src_reg, Byte dst_reg);
RunCode op_str(Object** rp, Stack* stack, Byte src_reg);
RunCode op_xcg(Object** rp, Byte reg1, Byte reg2);
RunCode op_pop(Stack* stack, const Object* object);
RunCode op_psh(Stack* stack, char* value);
RunCode op_asn(Stack* stack, ConcoctHashMap* map);
RunCode op_and(Stack* stack);
RunCode op_not(Stack* stack);
RunCode op_or(Stack* stack);
RunCode op_eql(Stack* stack);
RunCode op_neq(Stack* stack);
RunCode op_sle(Stack* stack);
RunCode op_sln(Stack* stack);
RunCode op_gt(Stack* stack);
RunCode op_gte(Stack* stack);
RunCode op_lt(Stack* stack);
RunCode op_lte(Stack* stack);
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
