
int token_type; //aktualni token
char token_attribute; //atribut aktualniho tokenu
token_t token;

int parse()
{
	DTree_t *d_tree;
	DTreeInit(d_tree);


	token_t *tok;
	while(se_mi_chce){

		int succes = get_token(tok);
		if(succes){
			DTreeInsert(d_tree,tok);
		}
	}
	


}

// simulace pravidla <program> -> <st-list>
int program(){

	SET_TOKEN();

	switch(token_type):
		//<program> -> <ID> <st-list>
		case TYPE_INT:
		case TYPE_FLOAT:
			math_until_EOL();
			

}

void SET_TOKEN(){
	get_token(*token);
	token_type = token->type;
}

int math_until_EOL(){

	int result;
	switch(token):
		case PLUS:
		case MINUS:
		case DIV:
		case MUL:
			SET_TOKEN();
			if(!is_num()){
				result = ERROR_4;
				return result;
			}
			if(result == is_err(math_until_EOL())){
				return result;
			}

		break;
		case TYPE_INT:
		case TYPE_FLOAT:
			SET_TOKEN();
			if(!is_operand()){
				result = ERROR_4;
				return result;
			}
			if(result == is_err(math_until_EOL())){
				return result;
			}
		break;

}
int is_err(int ret){
	switch(ret):
		case ERROR_1:
		case ERROR_2:
		case ERROR_3:
		case ERROR_4:
		case ERROR_5:
		case ERROR_6:
		case ERROR_9:
		case ERROR_99:
			return ret;
		default:
			return NO_ERROR;
}
bool is_operand(){
	return ((token_type >= PLUS && token_type <= MUL)||(token_type >= COMPARE && token_type <= NOTEQUAL));
}
bool is_num(){
	return (token_type == TYPE_INT || token_type == TYPE_FLOAT);
}
