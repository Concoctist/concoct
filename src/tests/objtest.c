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
#include "memory.h"
#include "types.h"

int main()
{
	init_store();
	Object* objects[512];
	for(size_t i = 0; i < 512; i++)
	{
		printf("Object store free/size: %zu/%zu\n\n", get_store_free_slots(), get_store_size());
		objects[i] = new_object("7");
	}

	for(size_t i = 0; i < 512; i++)
	{
		printf("%s\n", get_data_type(object_store.objects[i]));
		free_object(object_store.objects[i]);
	}
	puts("");

	Object* object = new_object("null");
	printf("Data type: %s\n", get_data_type(object));
	print_object_value(object);
	printf("%s\n\n", (char *)get_object_value(object));
	free_object(object);

	object = new_object("true");
	printf("Data type: %s\n", get_data_type(object));
	print_object_value(object);
	printf("%d\n\n", *(Bool *)get_object_value(object));
	free_object(object);

	object = new_object("100");
	printf("Data type: %s\n", get_data_type(object));
	print_object_value(object);
	printf("%" PRId32 "\n\n", *(Number *)get_object_value(object));
	free_object(object);

	object = new_object("5721452096347253");
	printf("Data type: %s\n", get_data_type(object));
	print_object_value(object);
	printf("%" PRId64 "\n\n", *(BigNum *)get_object_value(object));
	free_object(object);

	object = new_object("77.715");
	printf("Data type: %s\n", get_data_type(object));
	print_object_value(object);
	printf("%f\n\n", *(Decimal *)get_object_value(object));
	free_object(object);

	object = new_object("Greetings, Concocter!");
	printf("Data type: %s\nString value: %s\nString length: %zu\n", get_data_type(object), object->value.strobj.strval, object->value.strobj.length);
	print_object_value(object);
	printf("%s\n", (char *)get_object_value(object));

	puts("\nAfter realloc_string():");
	realloc_string(&object->value.strobj, "Farewell, Concocter!");
	printf("Data type: %s\nString value: %s\nString length: %zu\n", get_data_type(object), object->value.strobj.strval, object->value.strobj.length);
	print_object_value(object);
	printf("%s\n", (char *)get_object_value(object));
	Object *object2 = clone_object(object);

	puts("\nCloned object:");
	printf("Data type: %s\nString value: %s\nString length: %zu\n", get_data_type(object2), object2->value.strobj.strval, object2->value.strobj.length);
	free_object(object);
	free_object(object2);
	free_store();

	return 0;
}
