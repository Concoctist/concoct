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

#include <inttypes.h> // PRId32, PRId64
#include <stdio.h>    // printf(), puts()
#include "types.h"

// Returns string representation of data type
const char* get_data_type(DataType dtype)
{
	switch(dtype)
	{
		case NIL:     return "null";
		case BOOL:    return "boolean";
		case BYTE:    return "byte";
		case NUMBER:  return "number";
		case BIGNUM:  return "big number";
		case DECIMAL:   return "decimal";
		case STRING:  return "string";
		default:      return "unknown";
	}
}

// Displays value of object
void print_object_value(Object* object)
{
	switch(object->datatype)
	{
		case NIL:
			puts("null");
			break;
		case BOOL:
			if(object->value.boolean == false)
				puts("false");
			else
				puts("true");
			break;
		case BYTE:
			printf("%u\n", object->value.boolean);
			break;
		case NUMBER:
			printf("%" PRId32 "\n", object->value.number);
			break;
		case BIGNUM:
			printf("%" PRId64 "\n", object->value.bignum);
			break;
		case DECIMAL:
			printf("%f\n", object->value.decimal);
			break;
		case STRING:
			printf("%s\n", object->value.str.str);
			break;
	}
}
