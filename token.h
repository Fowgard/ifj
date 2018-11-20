#include <stdio.h>
#include <stdlib.h>
#include <string.h>

  


typedef struct Lexem {
	int lenght;
	char *word;	
} lexem_t;


typedef union //union smi mit pouze jednu z hodnot
{
	int integer;//hodnota kdyy je int
	double decimal;
	lexem_t string;//hodnota kdyz je string
	int keyword;//konstanta pro keyword
	int nil;//bool hodnota(je nebo neni to nil)
	
} attribute_t;


typedef struct Token {
   	int type;
  	attribute_t attribute;
} token_t;

void token_init(token_t *token);
void lexem_init(lexem_t *lexem);
void lexem_putchar(lexem_t *lexem, char symbol);
void set_token(int type, char *attribute);
void lexem_init(lexem_t *lexem);
void lexem_putchar(lexem_t *lexem, char symbol);
void set_type(token_t *token, int type);
int keyword_check(token_t *token, lexem_t *lexem);


#define FALSE 0
#define TRUE 1


#define END_OF_FILE 10
#define END_OF_LINE 11
#define END_OF_TOKEN 12
#define NIL 13
#define SUCCESS 14
#define WRONG_NUMBER_FORMAT 15
#define KONSTANT 16
#define COMMENT 17
#define COMMENT_END 18

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
#define STATE_NUMBER 42
#define STATE_COMMENT 43 // #
#define STATE_EQUALS 44 // = nevime jestli bude pokracovat druhe rovna se
#define STATE_LESSTHAN 45 // <
#define STATE_MORETHAN 46 // >
#define STATE_EXCLAM 47 // !
#define STATE_STRING_LITERAL 48


#define TYPE_KEYWORD 50
#define TYPE_IDENTIFIER 51
#define TYPE_INT 52

#define PLUS 60 // +
#define MINUS 61 // -
#define DIV 62 // /
#define MUL 63 // *
#define EQUALS 64 // = 
#define COMMA 65 // ,
#define LEFT_BRACKET 66 // (
#define RIGHT_BRACKET 67 // )
#define COMPARE 68 // ==
