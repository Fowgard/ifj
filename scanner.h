#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "token.h"

//konstanty pro jednotlive lexemy
//1 az 9 a 99 jsou chybove konstany, pouzit cokoliv krome nich
//def, do, else, end, if, not, nil, then, while


#define END_OF_FILE 10
#define END_OF_LINE 11
#define END_OF_TOKEN 12
#define NIL 13
#define COMMENT 14
#define SUCCESS 15
#define WRONG_NUMBER_FORMAT 16
#define NEJAKA_KONSTANTA 17


#define DEF 20
#define DO 21
#define END 22

#define IF 30
#define ELSE 31
#define THEN 32
#define NOT 33
#define WHILE 34


#define STATE_START 40
#define STATE_ID_KW 41
#define STATE_INT 42
#define STATE_COMMENT 43


#define TYPE_KEYWORD 50
#define TYPE_IDENTIFIER 51
#define TYPE_INT 52



#define INIT_SYMBOL '/'





void set_source_file(FILE *f);
int get_token(token_t *token);
int is_key_word(char *lexem);