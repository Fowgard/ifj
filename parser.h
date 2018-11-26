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



int program();
int rule_def();
int rule_stat();
int set_token_and_return();
int pop_token();
int rule_expr();
int is_err(int ret);
bool is_operand();
bool is_num();