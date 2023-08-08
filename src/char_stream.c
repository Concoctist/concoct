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
