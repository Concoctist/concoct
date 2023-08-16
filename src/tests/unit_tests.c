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

#include <assert.h> // assert()
#include <stdlib.h> // free()
#include <string.h> // strcmp(), strncpy()
#include "memory.h" // stringify()

void test_stringify()
{
  char* str = NULL;

  Byte byteval = 128;
  void* vptr = &byteval;
  stringify(&str, vptr, CCT_TYPE_BYTE);
  assert(strcmp(str, "128") == 0);
  free(str);

  Number numval = 42;
  vptr = &numval;
  stringify(&str, vptr, CCT_TYPE_NUMBER);
  assert(strcmp(str, "42") == 0);
  free(str);

  Decimal decimalval = 57.05;
  char dec[16];
  vptr = &decimalval;
  stringify(&str, vptr, CCT_TYPE_DECIMAL);
  if(str && strlen(str) > 5)
  {
    strncpy(dec, str, 5);
    dec[5] = '\0';
    assert(strcmp(dec, "57.05") == 0);
  }
  free(str);

  char* testval = "Foo bar baz";
  vptr = &testval;
  stringify(&str, vptr, CCT_TYPE_STRING);
  assert(strcmp(str, "Foo bar baz") == 0);
  free(str);

  Bool boolval = false;
  vptr = &boolval;
  stringify(&str, vptr, CCT_TYPE_BOOL);
  assert(strcmp(str, "false") == 0);
  free(str);

  boolval = true;
  vptr = &boolval;
  stringify(&str, vptr, CCT_TYPE_BOOL);
  assert(strcmp(str, "true") == 0);
  free(str);

  char* nullval = NULL;
  vptr = &nullval;
  stringify(&str, vptr, CCT_TYPE_NIL);
  assert(strcmp(str, "null") == 0);
  free(str);

  return;
}

int main()
{
  test_stringify();
  return 0;
}
