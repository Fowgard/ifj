//konstanty pro jednotlive lexemy
//1 az 9 a 99 jsou chybove konstany, pouzit cokoliv krome nich
//def, do, else, end, if, not, nil, then, while


#define END_OF_FILE 10
#define END_OF_LINE 11
#define NIL 12
#define COMMENT 13

#define DEF 20
#define DO 21
#define END 22

#define IF 30
#define ELSE 31
#define THEN 32
#define NOT 33
#define WHILE 34





void set_source_file(FILE *f);
int get_token(char token[]);