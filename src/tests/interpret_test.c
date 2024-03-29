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

#include "debug.h"
#include "hash_map.h"
#include "memory.h"
#include "vm/vm.h"

int main(void)
{
  Object* object = NULL;
  void* vptr = NULL;
  BigNum numval = -8675309;
  ConcoctHashMap* map = cct_new_hash_map(INITIAL_BUCKET_AMOUNT);
  debug_mode = true;
  init_vm();

  vm.instructions[14] = OP_XCG;
  vm.instructions[13] = OP_LOD;
  vm.instructions[12] = OP_STR;
  vm.instructions[11] = OP_MOV;
  vptr = &numval;
  object = new_object_by_type(vptr, CCT_TYPE_BIGNUM);
  vm.rp[R1] = object;
  vm.rp[RS] = object;
  vm.instructions[10] = OP_RET;
  numval = -5552424;
  vptr = &numval;
  push(vm.sp, new_object_by_type(vptr, CCT_TYPE_BIGNUM));
  vm.instructions[9] = OP_BNT;
  push(vm.sp, new_object("217"));
  push(vm.sp, new_object("107"));
  push(vm.sp, new_object("32"));
  vm.instructions[8] = OP_XOR;
  push(vm.sp, new_object("32"));
  push(vm.sp, new_object("8"));
  vm.instructions[7] = OP_AND;
  push(vm.sp, new_object("true"));
  push(vm.sp, new_object("true"));
  vm.instructions[6] = OP_NEG;
  push(vm.sp, new_object("35.5"));
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
  vm.instructions[15] = OP_CLR;
  vm.instructions[16] = OP_CLS;
  vm.instructions[17] = OP_END;

  interpret(map);
  cct_delete_hash_map(map);
  stop_vm();

  return 0;
}
