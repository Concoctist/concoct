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

#ifndef OPCODES_H
#define OPCODES_H

// Supported instruction set
typedef enum opcode
{
  OP_ADD, // add (+)
  OP_AND, // logical and (&&)
  OP_BND, // bitwise and (&)
  OP_BNT, // bitwise not/ones' complement (~)
  OP_BOR, // bitwise or (|)
  OP_CAL, // call
  OP_CLR, // clear registers
  OP_CLS, // clear stack
  OP_CMP, // compare
  OP_DEC, // decrement (--)
  OP_DIV, // divide (/)
  OP_END, // marks end of VM instructions
  OP_ENT, // entry point
  OP_EQL, // equal to (==)
  OP_EXT, // exit
  OP_FLS, // false
  OP_GT,  // greater than (>)
  OP_GTE, // greater than or equal to (>=)
  OP_HLT, // halt
  OP_INC, // increment (++)
  OP_JMC, // jump conditional
  OP_JMP, // jump
  OP_JMZ, // jump zero
  OP_LNE, // loop not equal
  OP_LNZ, // loop not zero
  OP_LOD, // load (from memory to register)
  OP_LOE, // loop equal
  OP_LOP, // loop
  OP_LOZ, // loop zero
  OP_LT,  // less than (<)
  OP_LTE, // less than or equal to (<=)
  OP_MOD, // modulo (%)
  OP_MOV, // move (from register to register)
  OP_MUL, // multiply (*)
  OP_NEG, // negative
  OP_NEQ, // not equal to (!=)
  OP_NOP, // no op
  OP_NOT, // logical not/negation (!)
  OP_NUL, // null
  OP_OR,  // logical or (||)
  OP_POP, // pop
  OP_POS, // positive
  OP_POW, // power/exponent (**)
  OP_PSH, // push
  OP_RET, // return
  OP_SHL, // bitshift left (<<)
  OP_SHR, // bitshift right (>>)
  OP_SLE, // string length equal to ($=)
  OP_SLN, // string length not equal to ($!)
  OP_STR, // store (to memory from register)
  OP_SUB, // subtract (-)
  OP_SYS, // system
  OP_TRU, // true
  OP_TST, // test
  OP_XCG, // exchange/swap
  OP_XOR  // bitwise exclusive or (^)
} Opcode;

// Returns opcode constant based on numeric ID
const char* get_mnemonic(Opcode oc);

#endif // OPCODES_H
