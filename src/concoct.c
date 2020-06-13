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

#include <errno.h>   // errono
#include <stdio.h>   // FILE, fclose(), fprintf(), printf(), puts()
#include <stdlib.h>  // exit()
#include <string.h>  // strerror(), strlen()
#include "version.h" // version

#include "concoct_lexer.h"
#include "vm.h"

#define LINE_LENGTH 256

int main(int argc, char **argv)
{
  FILE *input_file;
  //char line[LINE_LENGTH];

  printf("Concoct v%s\n", VERSION);

  if(argc < 2)
  {
    puts("No input file!");
    exit(1);
  }

  if(argc > 2)
  {
    puts("Too many arguments!");
    exit(1);
  }

  input_file = fopen(argv[1] , "r");

  if(input_file == NULL)
  {
    fprintf(stderr, "Error opening %s: %s\n", argv[1], strerror(errno));
    exit(1);
  }

  // For now, print each line in a file with corresponding line number and character count
  // ToDo: Parse tokens from .ct source file
  /*
  int line_number = 0;
  while(fgets (line, LINE_LENGTH, input_file) != NULL)
  {
     line_number++;
     printf("%i\t(%i)\t%s", line_number, strlen(line), line);
  }
  */
  // Creates a new lexer and iterates through the tokens
  struct ConcoctLexer lexer = cct_new_lexer(input_file);
  struct ConcoctToken token = cct_next_token(&lexer);
  // Continues printing until an EOF is reached
  while(token.type != CCT_TOKEN_EOF)
  {
    if(lexer.error != NULL)
    {
      printf("Error on line %i\n", token.line_number);
      printf(lexer.error, token.text);
      free(token.text);
      break;
    }
    printf("[%i] %s : %s\n", token.line_number, token.text, cct_token_type_to_string(token.type));
    // Token.text is allocated on the heap, so it's cleaned up here
    free(token.text);
    token = cct_next_token(&lexer);
  }

  fclose(input_file);

  return 0;
}
