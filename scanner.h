#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "token.h"

//konstanty pro jednotlive lexemy
//1 az 9 a 99 jsou chybove konstany, pouzit cokoliv krome nich
//def, do, else, end, if, not, nil, then, while






void set_source_file(FILE *f);
int get_token(token_t *token);
int is_keyword(char *lexem);