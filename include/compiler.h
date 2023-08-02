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

#ifndef COMPILER_H
#define COMPILER_H

#include <stdbool.h> // bool
#include "parser.h"

#define MAX_QUEUE_CAPACITY ((size_t)256)
//static const size_t MAX_QUEUE_CAPACITY = CCT_NODE_COUNT_PER_BLOCK;

typedef struct node_queue
{
  int front;
  int back;
  size_t count;
  ConcoctNode* nodes[MAX_QUEUE_CAPACITY];
} Queue;

// Initializes queue
void init_queue(Queue* queue);

// Is queue empty?
bool is_empty(Queue* queue);

// Is queue full?
bool is_full(Queue* queue);

// Returns size of queue
size_t size(Queue* queue);

// Returns node at the back of queue
ConcoctNode* back(Queue* queue);

// Returns node at the front of queue
ConcoctNode* front(Queue* queue);

// Returns and removes next node from queue
void dequeue(Queue* queue, ConcoctNode** node);

// Inserts new node into queue
void enqueue(Queue* queue, ConcoctNode* node);

// Translates lexer/parser tokens to VM instructions
void compile(ConcoctNodeTree* tree);

#endif // COMPILER_H
