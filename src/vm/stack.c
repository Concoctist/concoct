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

#include <stdio.h>    // fprintf(), stderr
#include <stdlib.h>   // EXIT_FAILURE
#include "debug.h"
#include "vm/stack.h"

void init_stack(Stack* stack)
{
  stack->count = 0;
  stack->top = -1;
  if(debug_mode)
    debug_print("Stack initialized with %zu slots.", MAX_STACK_CAPACITY);
  return;
}

// Returns object at top of stack without removal
Object* peek(Stack* stack)
{
  if(stack->top == -1)
  {
    if(debug_mode)
      debug_print("peek() called on empty stack!");
    return NULL;
  }
  if(debug_mode)
    debug_print("peek() called on stack for object of type %s. Stack currently contains %zu objects.", get_data_type(stack->objects[stack->top]), stack->count);
  return stack->objects[stack->top];
}

// Returns and removes object at top of stack
Object* pop(Stack* stack)
{
  if(stack->top == -1)
  {
    fprintf(stderr, "Stack underflow occurred!\n");
    return NULL;
  }
  stack->count--;
  if(debug_mode)
    debug_print("pop() called on stack for object of type %s. Stack now contains %zu objects.", get_data_type(stack->objects[stack->top]), stack->count);
  return stack->objects[stack->top--];
}

// Pushes new object on top of stack
void push(Stack* stack, Object* object)
{
  if(stack->top >= ((int)MAX_STACK_CAPACITY - 1))
  {
    fprintf(stderr, "Stack overflow occurred!\n");
    return;
  }
  stack->count++;
  stack->objects[++stack->top] = object;
  if(debug_mode)
    debug_print("push() called on stack for object of type %s. Stack now contains %zu objects.", get_data_type(stack->objects[stack->top]), stack->count);
  return;
}
