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

#include <inttypes.h>
#include <stdio.h>
#include "types.h"

int main()
{
	Object object;

	convert_type(&object, "null");
	printf("Data type: %s\n", get_data_type(object.datatype));
	print_object_value(&object);
	puts("");

	convert_type(&object, "true");
	printf("Data type: %s\n", get_data_type(object.datatype));
	print_object_value(&object);
	puts("");

	convert_type(&object, "100");
	printf("Data type: %s\n", get_data_type(object.datatype));
	print_object_value(&object);
	puts("");

	convert_type(&object, "5721452096347253");
	printf("Data type: %s\n", get_data_type(object.datatype));
	print_object_value(&object);
	puts("");

	convert_type(&object, "77.715");
	printf("Data type: %s\n", get_data_type(object.datatype));
	print_object_value(&object);
	puts("");

	convert_type(&object, "Greetings, Concocter!");
	printf("Data type: %s\n", get_data_type(object.datatype));
	print_object_value(&object);
	free_string(&object.value.strobj);
	puts("");

	object.datatype = NIL;
	object.value.nullval = NULL;
	printf("Data type: %s\n", get_data_type(object.datatype));
	print_object_value(&object);
	printf("%s\n\n", (char *)get_object_value(&object));

	object.datatype = BOOL;
	object.value.boolval = true;
	printf("Data type: %s\n", get_data_type(object.datatype));
	print_object_value(&object);
	printf("%d\n\n", *(Bool *)get_object_value(&object));

	object.datatype = BYTE;
	object.value.byteval = 7;
	printf("Data type: %s\n", get_data_type(object.datatype));
	print_object_value(&object);
	printf("%u\n\n", *(Byte *)get_object_value(&object));

	object.datatype = NUMBER;
	object.value.numval = 473921;
	printf("Data type: %s\n", get_data_type(object.datatype));
	print_object_value(&object);
	printf("%" PRId32 "\n\n", *(Number *)get_object_value(&object));

	object.datatype = BIGNUM;
	object.value.bignumval = 7233720368547758;
	printf("Data type: %s\n", get_data_type(object.datatype));
	print_object_value(&object);
	printf("%" PRId64 "\n\n", *(BigNum *)get_object_value(&object));

	object.datatype = DECIMAL;
	object.value.decimalval = 7.0;
	printf("Data type: %s\n", get_data_type(object.datatype));
	print_object_value(&object);
	printf("%f\n\n", *(Decimal *)get_object_value(&object));

	// Strings will be abstracted later for easier use
	object.datatype = STRING;
	new_string(&object.value.strobj, "test string");
	printf("Data type: %s\nString value: %s\nString length: %zu\n", get_data_type(object.datatype), object.value.strobj.strval, object.value.strobj.length);
	printf("%s\n", (char *)get_object_value(&object));
	free_string(&object.value.strobj);
	return 0;
}
