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

#include <errno.h>       // errno
#include <stdlib.h>      // malloc()
#include <string.h>      // strerror()
#include "char_stream.h"

ConcoctCharStream* cct_new_file_char_stream(FILE* in_file)
{
  ConcoctCharStream* stream = malloc(sizeof(ConcoctCharStream));

  if(stream == NULL)
  {
    fprintf(stderr, "Error allocating memory for lexer: %s\n", strerror(errno));
    return NULL;
  }

  stream->type = CCT_CHAR_STREAM_FILE;
  stream->input.file_input = in_file;
  stream->index = 0;
  return stream;
}

ConcoctCharStream* cct_new_string_char_stream(const char* in_string)
{
  ConcoctCharStream* stream = malloc(sizeof(ConcoctCharStream));

  if(stream == NULL)
  {
    fprintf(stderr, "Error allocating memory for lexer: %s\n", strerror(errno));
    return NULL;
  }

  stream->type = CCT_CHAR_STREAM_STRING;
  stream->input.string_input = in_string;
  stream->index = 0;
  return stream;
}

char cct_get_char_from_stream(ConcoctCharStream* stream)
{
  if(!cct_char_stream_eof(stream))
  {
    stream->index += 1;
    if(stream->type == CCT_CHAR_STREAM_STRING)
    {
      char ch = stream->input.string_input[stream->index - 1];
      return ch;
    }
    return (char)getc(stream->input.file_input);
  }
  return '\0';
}

int cct_char_stream_eof(ConcoctCharStream* stream)
{
  if(stream->type == CCT_CHAR_STREAM_STRING)
  {
    return stream->input.string_input[stream->index] == '\0';
  }
  return feof(stream->input.file_input);
}

void cct_delete_char_stream(ConcoctCharStream* stream)
{
  free(stream);
}
