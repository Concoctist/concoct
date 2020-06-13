/*
 * Concoct - An imperative, dynamically-typed, interpreted, general-purpose programming language
 * Copyright (C) 2020 BlakeTheBlock, Lloyd Dilley
 * https://github.com/FragLand/concoct
 *
 * This file is part of Concoct.
 *
 * Concoct is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version
 * 3 of the License, or (at your option) any later version.
 *
 * Concoct is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Concoct. If not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>   // errono
#include <stdio.h>   // FILE, fclose(), fprintf(), printf(), puts()
#include <stdlib.h>  // exit()
#include <string.h>  // strerror(), strlen()
#include "version.h" // version

#include "concoct_lexer.h"

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