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

#include "vm/opcodes.h"

// Returns opcode constant based on numeric ID
const char* get_mnemonic(Opcode oc)
{
  switch(oc)
  {
    case OP_ADD: return "OP_ADD";    // add (+)
    case OP_AND: return "OP_AND";    // logical and (&&)
    case OP_BND: return "OP_BND";    // bitwise and (&)
    case OP_BNT: return "OP_BNT";    // bitwise not/ones' complement (~)
    case OP_BOR: return "OP_BOR";    // bitwise or (|)
    case OP_CAL: return "OP_CAL";    // call
    case OP_CMP: return "OP_CMP";    // compare
    case OP_DEC: return "OP_DEC";    // decrement (--)
    case OP_DIV: return "OP_DIV";    // divide (/)
    case OP_END: return "OP_END";    // marks end of VM instructions
    case OP_ENT: return "OP_ENT";    // entry point
    case OP_EQL: return "OP_EQL";    // equal to (==)
    case OP_EXT: return "OP_EXT";    // exit
    case OP_FLS: return "OP_FLS";    // false
    case OP_GT:  return "OP_GT";     // greater than (>)
    case OP_GTE: return "OP_GTE";    // greater than or equal to (>=)
    case OP_HLT: return "OP_HLT";    // halt
    case OP_INC: return "OP_INC";    // increment (++)
    case OP_JMC: return "OP_JMC";    // jump conditional
    case OP_JMP: return "OP_JMP";    // jump
    case OP_JMZ: return "OP_JMZ";    // jump zero
    case OP_LNE: return "OP_LNE";    // loop not equal
    case OP_LNZ: return "OP_LNZ";    // loop not zero
    case OP_LOE: return "OP_LOE";    // loop equal
    case OP_LOP: return "OP_LOP";    // loop
    case OP_LOR: return "OP_LOR";    // logical or (||)
    case OP_LOZ: return "OP_LOZ";    // loop zero
    case OP_LT:  return "OP_LT";     // less than (<)
    case OP_LTE: return "OP_LTE";    // less than or equal to (<=)
    case OP_MOD: return "OP_MOD";    // modulo (%)
    case OP_MOV: return "OP_MOV";    // move
    case OP_MUL: return "OP_MUL";    // multiply (*)
    case OP_NEG: return "OP_NEG";    // negative
    case OP_NEQ: return "OP_NEQ";    // not equal to (!=)
    case OP_NOP: return "OP_NOP";    // no op
    case OP_NOT: return "OP_NOT";    // logical not/negation (!)
    case OP_NUL: return "OP_NUL";    // null
    case OP_POP: return "OP_POP";    // pop
    case OP_POS: return "OP_POS";    // positive
    case OP_POW: return "OP_POW";    // power/exponent (**)
    case OP_PSH: return "OP_PSH";    // push
    case OP_RET: return "OP_RET";    // return
    case OP_SHL: return "OP_SHL";    // bitshift left (<<)
    case OP_SHR: return "OP_SHR";    // bitshift right (>>)
    case OP_STR: return "OP_STR";    // store
    case OP_SUB: return "OP_SUB";    // subtract (-)
    case OP_SYS: return "OP_SYS";    // system
    case OP_TRU: return "OP_TRU";    // true
    case OP_TST: return "OP_TST";    // test
    case OP_XOR: return "OP_XOR";    // bitwise exclusive or (^)
    default:     return "UNDEFINED"; // unsupported opcode
  }
}
