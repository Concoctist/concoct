#ifndef CCT_CHAR_STREAM_H
#define CCT_CHAR_STREAM_H

#include <stdio.h> // FILE
#include <stdlib.h>
#include <string.h>

typedef enum concoct_char_stream_type
{
  CCT_CHAR_STREAM_FILE,
  CCT_CHAR_STREAM_STRING
} ConcoctCharStreamType;


typedef struct concoct_char_stream
{
  ConcoctCharStreamType type;
  union
  {
    FILE* file_input;
    const char* string_input;
  } input;
  int index;
} ConcoctCharStream;

// Character stream constructors
ConcoctCharStream* cct_new_file_char_stream(FILE* in_file);
ConcoctCharStream* cct_new_string_char_stream(const char* in_string);

char cct_get_char_from_stream(ConcoctCharStream* stream);
int cct_char_stream_eof(ConcoctCharStream* stream);
void cct_delete_char_stream(ConcoctCharStream* stream);

#endif