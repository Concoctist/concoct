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

#ifndef VM_H
#define VM_H

#include "stack.h"
#include "types.h"      // BigNum, Byte
#include "vm/opcodes.h" // Opcode

#define REGISTER_AMOUNT ((uint8_t)17)
static const uint8_t REGISTER_EMPTY = 127;
static const size_t INSTRUCTION_STORE_SIZE = 128;

typedef struct vm
{
  Opcode* instructions;               // instructions to execute
  Object* registers[REGISTER_AMOUNT]; // registers
  Object** rp;                        // register pointer
  Stack stack;                        // stack structure
  Stack* sp;                          // stack pointer/top item of stack
  Opcode* ip;                         // instruction pointer/program counter
} VM;
extern VM vm;

// Register names/indexes
static const Byte R0 = 0;
static const Byte R1 = 1;
static const Byte R2 = 2;
static const Byte R3 = 3;
static const Byte R4 = 4;
static const Byte R5 = 5;
static const Byte R6 = 6;
static const Byte R7 = 7;
static const Byte R8 = 8;
static const Byte R9 = 9;
static const Byte R10 = 10;
static const Byte R11 = 11;
static const Byte R12 = 12;
static const Byte R13 = 13;
static const Byte R14 = 14;
static const Byte R15 = 15;
static const Byte RS = 16;  // result
extern Opcode** IP;         // instruction pointer
extern Object** RP;         // register pointer
extern Stack** SP;          // stack pointer

typedef enum
{
  RUN_SUCCESS,
  RUN_ERROR
} RunCode;

// Clears instructions
void clear_instructions();

// Reverses instructions since they should be in a LIFO arrangement
void reverse_instructions(size_t ic);

// Prints register values
void print_registers();

// Initializes virtual machine
void init_vm();

// Stops virtual machine
void stop_vm();

// Interprets code
RunCode interpret();

#endif // VM_H
