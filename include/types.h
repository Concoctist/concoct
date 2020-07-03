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

#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h> // bool
#include <stddef.h>  // NULL, size_t
#include <stdint.h>  // uint8_t, int32_t, int64_t

// Concoct data types
typedef bool Bool;         // true/false
typedef uint8_t Byte;      // unsigned char
typedef int32_t Number;    // signed long integer
typedef int64_t BigNum;    // signed long long integer
typedef double Decimal;    // decimal
typedef struct cct_string  // string
{
	size_t length;
	char *strval;
} String;

typedef enum data_type
{
	NIL,
	BOOL,
	BYTE,
	NUMBER,
	BIGNUM,
	DECIMAL,
	STRING
} DataType;

// Concoct object
typedef struct object
{
	DataType datatype;
	union
	{
		char* nullval;
		Bool boolval;
		Byte byteval;
		Number numval;
		BigNum bignumval;
		Decimal decimalval;
		String strobj; // strval is contained in String struct
	} value;
} Object;

// Populate String struct
void new_string(String* strobj, char* str);

// Free string
void free_string(String* strobj);

// Populate Object struct
Object* new_object(char* value);

// Free object
void free_object(Object* object);

// Returns string representation of data type
const char* get_data_type(Object* object);

// Returns value of object
void* get_object_value(Object* object);

// Displays value of object
void print_object_value(Object* object);

// Converts string to applicable type
void convert_type(Object* object, char* value);

#endif // TYPES_H
