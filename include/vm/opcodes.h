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

#ifndef OPCODES_H
#define OPCODES_H

/* Supported instruction set */
typedef enum opcode
{
  OP_ADD, // add
  OP_AND, // and
  OP_CAL, // call
  OP_CMP, // compare
  OP_DEC, // decrement
  OP_DIV, // divide
  OP_ENT, // entry point
  OP_EXT, // exit
  OP_FLS, // false
  OP_HLT, // halt
  OP_INC, // increment
  OP_JMC, // jump conditional
  OP_JMP, // jump
  OP_JMZ, // jump zero
  OP_LNE, // loop not equal
  OP_LNZ, // loop not zero
  OP_LOE, // loop equal
  OP_LOP, // loop
  OP_LOZ, // loop zero
  OP_MOD, // modulus
  OP_MOV, // move
  OP_MUL, // multiply
  OP_NOP, // no op
  OP_NOT, // not
  OP_NUL, // null
  OP_OR,  // or
  OP_POP, // pop
  OP_POW, // power/exponent
  OP_PSH, // push
  OP_RET, // return
  OP_SHL, // bitshift left
  OP_SHR, // bitshift right
  OP_STR, // store
  OP_SUB, // subtract
  OP_SYS, // system
  OP_TRU, // true
  OP_TST, // test
  OP_XOR  // exclusive or
} Opcode;

/* Returns opcode constant based on numeric ID */
const char* get_mnemonic(opcode oc);

#endif /* OPCODES_H */
