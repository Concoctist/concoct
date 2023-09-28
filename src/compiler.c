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
#include "debug.h"    // debug_mode, debug_print()
#include "memory.h"   // new_object(), new_object_by_type()
#include "queue.h"
#include "vm/vm.h"    // interpret(), reverse_instructions()

// Swap last 2 stack objects to fix order if first instruction is a binary operation
void swap_last_operands(Opcode oc)
{
  Object* object1 = NULL;
  Object* object2 = NULL;
  if(!is_binary_operation(oc))
    return;
  if(debug_mode)
    debug_print("Swapping top 2 objects of stack...");
  object1 = pop(vm.sp);
  object2 = pop(vm.sp);
  push(vm.sp, object1);
  push(vm.sp, object2);
  return;
}

// Translates lexer/parser tokens to VM instructions
void compile(ConcoctNodeTree* tree, ConcoctHashMap* map)
{
  ConcoctNode* root = tree->root;
  Queue queue;
  Queue* pqueue = &queue;
  size_t ic = 0; // instruction count

  if(root == NULL)
    return;

  // Use a level-order algorithm to walk the parser tree
  init_queue(pqueue);
  enqueue(pqueue, root);
  while(!is_empty(pqueue))
  {
    ConcoctNode* current = NULL;
    dequeue(pqueue, (void **)&current);
    // ToDo: Add remaining cases
    switch(current->token.type)
    {
      case CCT_TOKEN_ADD:
        vm.instructions[ic] = OP_ADD;
        ic++;
        break;
      case CCT_TOKEN_ADD_ASSIGN:
        vm.instructions[ic] = OP_ADD;
        ic++;
        vm.instructions[ic] = OP_ASN;
        ic++;
        break;
      case CCT_TOKEN_AND:
        vm.instructions[ic] = OP_AND;
        ic++;
        break;
      case CCT_TOKEN_ASSIGN:
        vm.instructions[ic] = OP_ASN; // this operation should precede an identifier (CCT_TOKEN_IDENTIFIER)
        ic++;
        break;
      case CCT_TOKEN_BIN_AND:
        vm.instructions[ic] = OP_BND;
        ic++;
        break;
      case CCT_TOKEN_BIN_OR:
        vm.instructions[ic] = OP_BOR;
        ic++;
        break;
      case CCT_TOKEN_BIN_XOR:
        vm.instructions[ic] = OP_XOR;
        ic++;
        break;
      case CCT_TOKEN_CHAR:
        push(vm.sp, new_object_by_type(current->text, CCT_TYPE_BYTE));
        break;
      case CCT_TOKEN_DEC:
        vm.instructions[ic] = OP_DEC;
        ic++;
        break;
      case CCT_TOKEN_DIV:
        vm.instructions[ic] = OP_DIV;
        ic++;
        break;
      case CCT_TOKEN_DIV_ASSIGN:
        // OP_DIV + OP_ASN
        break;
      case CCT_TOKEN_EQUAL:
        vm.instructions[ic] = OP_EQL;
        ic++;
        break;
      case CCT_TOKEN_EXP:
        vm.instructions[ic] = OP_POW;
        ic++;
        break;
      case CCT_TOKEN_EXP_ASSIGN:
        // OP_POW + OP_ASN
        break;
      case CCT_TOKEN_FALSE:
        vm.instructions[ic] = OP_FLS;
        ic++;
        break;
      case CCT_TOKEN_FLOAT:
        push(vm.sp, new_object_by_type(current->text, CCT_TYPE_DECIMAL));
        break;
      case CCT_TOKEN_GREATER:
        vm.instructions[ic] = OP_GT;
        ic++;
        break;
      case CCT_TOKEN_GREATER_EQUAL:
        vm.instructions[ic] = OP_GTE;
        ic++;
        break;
      case CCT_TOKEN_IDENTIFIER:
        if(!cct_hash_map_has_key(map, current->text))
        {
          push(vm.sp, new_object_by_type(current->text, CCT_TYPE_STRING));
        }
        else
        {
          // Identifier already exists. Flag the original object for garbage collection and delete the value.
          Object* object = cct_hash_map_get(map, current->text);
          object->is_flagged=true;
          cct_hash_map_delete_entry(map, current->text);
        }
        break;
      case CCT_TOKEN_INC:
        vm.instructions[ic] = OP_INC;
        ic++;
        break;
      case CCT_TOKEN_INT:
        push(vm.sp, new_object(current->text));
        break;
      case CCT_TOKEN_LESS:
        vm.instructions[ic] = OP_LT;
        ic++;
        break;
      case CCT_TOKEN_LESS_EQUAL:
        vm.instructions[ic] = OP_LTE;
        ic++;
        break;
      case CCT_TOKEN_MOD:
        vm.instructions[ic] = OP_MOD;
        ic++;
        break;
      case CCT_TOKEN_MOD_ASSIGN:
        // OP_MOD + OP_ASN
        break;
      case CCT_TOKEN_MUL:
        vm.instructions[ic] = OP_MUL;
        ic++;
        break;
      case CCT_TOKEN_MUL_ASSIGN:
        // OP_MUL + OP_ASN
        break;
      case CCT_TOKEN_NOT:
        vm.instructions[ic] = OP_NOT;
        ic++;
        break;
      case CCT_TOKEN_NOT_EQUAL:
        vm.instructions[ic] = OP_NEQ;
        ic++;
        break;
      case CCT_TOKEN_NEWLINE:
        // Ignore whitespace
        break;
      case CCT_TOKEN_OR:
        vm.instructions[ic] = OP_OR;
        ic++;
        break;
      case CCT_TOKEN_SHL:
        vm.instructions[ic] = OP_SHL;
        ic++;
        break;
      case CCT_TOKEN_SHR:
        vm.instructions[ic] = OP_SHR;
        ic++;
        break;
      case CCT_TOKEN_STRING:
        push(vm.sp, new_object_by_type(current->text, CCT_TYPE_STRING));
        break;
      case CCT_TOKEN_STRLEN_EQUAL:
        vm.instructions[ic] = OP_SLE;
        ic++;
        break;
      case CCT_TOKEN_STRLEN_NOT_EQUAL:
        vm.instructions[ic] = OP_SLN;
        ic++;
        break;
      case CCT_TOKEN_SUB:
        vm.instructions[ic] = OP_SUB;
        ic++;
        break;
      case CCT_TOKEN_SUB_ASSIGN:
        // OP_SUB + OP_ASN
        break;
      case CCT_TOKEN_TRUE:
        vm.instructions[ic] = OP_TRU;
        ic++;
        break;
      case CCT_TOKEN_UNARY_MINUS:
        vm.instructions[ic] = OP_NEG;
        ic++;
        break;
      default:
        fprintf(stderr, "Unable to handle token: %s\n", cct_token_type_to_string(current->token.type));
        break;
    }
    for(size_t i = 0; i < current->child_count; i++)
      enqueue(pqueue, current->children[i]);
  }

  if(ic > 0)
  {
    reverse_instructions(ic);
    swap_last_operands(vm.instructions[0]);
    vm.instructions[ic] = OP_END; // append end instruction
    interpret(map);
  }

  return;
}
