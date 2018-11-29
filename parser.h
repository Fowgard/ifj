#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"
//#include "token.h"
#include "symtable.h"

#define TYPE_FUNCTION 85


#define NO_ERROR 500
#define ERROR_1 501
#define ERROR_2 502
#define ERROR_3 503
#define ERROR_4 504
#define ERROR_5 505
#define ERROR_6 506
#define ERROR_9 507
#define ERROR_99 508


#define ALREADY_DEF 530
#define WITHOUT_TYPE 531



int program();
void call_generator(int resu);
int rule_string();
int rule_expresion_pusher();
int zjisti_co_je_id();
int rule_definice_promene();
int rule_def();
int set_token_and_return();
int pop_token();
int rule_expr();
int is_err(int ret);
bool is_operator();
bool is_num();
void init_parser();