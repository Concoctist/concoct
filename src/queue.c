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
#include "queue.h"

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

// Returns true if queue is empty
bool is_empty(const Queue* queue)
{
  return queue->count == 0;
}

// Returns true if queue is full
bool is_full(const Queue* queue)
{
  return queue->count == MAX_QUEUE_CAPACITY;
}

// Returns size of queue
size_t size(const Queue* queue)
{
  return queue->count;
}

// Returns object at the back of queue
void* back(const Queue* queue)
{
  if(is_empty(queue))
    return NULL;
  return queue->objects[queue->back];
}

// Returns object at the front of queue
void* front(const Queue* queue)
{
  if(is_empty(queue))
    return NULL;
  return queue->objects[queue->front];
}

// Returns and removes next object from queue
void dequeue(Queue* queue, void** object)
{
  if(is_empty(queue))
    return;
  *object = queue->objects[queue->front];
  queue->front = (queue->front + 1) % MAX_QUEUE_CAPACITY;
  queue->count--;
  return;
}

// Inserts new object into queue
void enqueue(Queue* queue, void* object)
{
  if(is_full(queue))
    return;
  queue->back = (queue->back + 1) % MAX_QUEUE_CAPACITY;
  queue->objects[queue->back] = object;
  queue->count++;
  return;
}
