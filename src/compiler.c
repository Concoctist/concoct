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

#include <stdio.h>    // fprintf()
#include "compiler.h"
#include "debug.h"    // debug_print()
#include "memory.h"   // new_object(), new_object_by_type()
#include "vm/vm.h"    // interpret()

// Initializes queue
void init_queue(Queue* queue)
{
  queue->count = 0;
  queue->back = MAX_QUEUE_CAPACITY - 1;
  queue->front = 0;
  if(debug_mode)
    debug_print("Queue initialized with %zu slots.", MAX_QUEUE_CAPACITY);
  return;
}

// Is queue empty?
bool is_empty(Queue* queue)
{
  return queue->count == 0;
}

// Is queue full?
bool is_full(Queue* queue)
{
  return queue->count == MAX_QUEUE_CAPACITY;
}

// Returns size of queue
size_t size(Queue* queue)
{
  return queue->count;
}

// Returns node at the back of queue
ConcoctNode* back(Queue* queue)
{
  if(is_empty(queue))
    return NULL;
  return queue->nodes[queue->back];
}

// Returns node at the front of queue
ConcoctNode* front(Queue* queue)
{
  if(is_empty(queue))
    return NULL;
  return queue->nodes[queue->front];
}

// Returns and removes next node from queue
void dequeue(Queue* queue, ConcoctNode** node)
{
  if(is_empty(queue))
    return;
  *node = queue->nodes[queue->front];
  queue->front = (queue->front + 1) % MAX_QUEUE_CAPACITY;
  queue->count--;
  return;
}

// Inserts new node into queue
void enqueue(Queue* queue, ConcoctNode* node)
{
  if(is_full(queue))
    return;
  queue->back = (queue->back + 1) % MAX_QUEUE_CAPACITY;
  queue->nodes[queue->back] = node;
  queue->count++;
  return;
}

// Translates lexer/parser tokens to VM instructions
void compile(ConcoctNodeTree* tree)
{
  ConcoctNode* root = tree->root;
  Queue queue;
  Queue* pqueue = &queue;

  if(root == NULL)
    return;

  // Use a level-order algorithm to walk the parser tree
  init_queue(pqueue);
  enqueue(pqueue, root);
  while(!is_empty(pqueue))
  {
    ConcoctNode* current = NULL;
    dequeue(pqueue, &current);
    // ToDo: Add remaining cases
    switch(current->token.type)
    {
      case CCT_TOKEN_ADD:
        //vm.instructions[x] = OP_ADD;
        break;
      case CCT_TOKEN_ASSIGN:
        break;
      case CCT_TOKEN_FLOAT:
        push(vm.sp, new_object_by_type(current->text, DECIMAL));
        break;
      case CCT_TOKEN_IDENTIFIER:
        break;
      case CCT_TOKEN_INT:
        push(vm.sp, new_object(current->text));
        break;
      case CCT_TOKEN_NEWLINE:
        // Ignore whitespace
        break;
      case CCT_TOKEN_STRING:
        push(vm.sp, new_object_by_type(current->text, STRING));
        break;
      default:
        fprintf(stderr, "Unable to handle token: %s\n", cct_token_type_to_string(current->token.type));
        break;
    }
    for(int i = 0; i < current->child_count; i++)
      enqueue(pqueue, current->children[i]);
  }
  // ToDo: Append OP_END and uncomment interpret()
  //vm.instructions[x] = OP_END;
  //interpret();
  return;
}
