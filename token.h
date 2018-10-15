#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//int key_counter = 0;

/*


 	char *key;
   char type;
   int lenght;
   char *lexem[];

*/
typedef struct Token {
  	int *attribute;// TODO implementovat hash table
   	int type;
} token_t;  


typedef struct Lexem {
	int lenght;
	char *word;	
} lexem_t;


void token_init(token_t *token);
void lexem_init(lexem_t *lexem);
void lexem_putchar(lexem_t *lexem, char symbol);
void set_token(int type, char *attribute);

#define END_OF_FILE 10
#define END_OF_LINE 11
#define END_OF_TOKEN 12
#define NIL 13
#define SUCCESS 14
#define WRONG_NUMBER_FORMAT 15
#define NEJAKA_KONSTANTA 16


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