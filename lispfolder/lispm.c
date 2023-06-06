//test
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define heapSize 10000 
#define stackEvalSize 10000 


#define _REDTEXT_ "\033[31m"
#define _GREENTEXT_ "\033[32m"
#define _YELLOWTEXT_ "\033[1;33m"
#define _NORMALTEXT_ "\033[0m"


// snke_case
// CamlCase
// dromedaryCase - pascalCase

enum type {INTEGER, SYMBOL, LIST, STRING, NU0MLIST};
enum commands {EVAL, CONS, APPLY1,APPLY2, APPLY};

struct lispList;

union element{
	int num;
	char symbol;  
	struct lispList* list;
	struct evalData* evalData;
}  info;

struct evalData{
	union element info;	
	enum type t;
};


struct lispList{
	union element info;
	int useful; 
	enum type t; 
	struct lispList* next; 
};


struct tokenizeList{
	char* info;
	struct tokenizeList* next;
};



struct lispList* heap;
struct lispList* freeList;

struct evalData stackEval[stackEvalSize];
struct evalData evaled[stackEvalSize];
enum commands commandList[stackEvalSize];

int stackEvalIndex = 0;
int evaledIndex = 0;
int commandListIndex = 0;

struct lispList* res;
/*
stack: (* ( + 1 2 ) 4)
evaled:
comands: EVAL APLLY

stack:  ( + 1 2 ) 4
evaled: *
comands: EVAL EVAL APPLY

stack:   1 2  4
evaled: + * 
comands: EVAL EVAL APPLY EVAL APPLY

stack:  2  4
evaled: 1 + * 
comands: EVAL APPLY EVAL APPLY

stack:   1 2  4
evaled: 2 1 + * 
comands:  APPLY EVAL APPLY

stack:   4
evaled:  3 * 
comands: EVAL APPLY

stack:   
evaled:  4 3 * 
comands: APPLY

stack:   
evaled:  12 
comands: 

*/

void printError(const char* text, const char* function);


void printElement(union element info, enum type t);
void printList(struct lispList* list);

//////////////////////////////////////////////////Free////////////////////////////////////////////////////////////
//void* freeData()

struct tokenizeList* freeFirstStackShallow(struct tokenizeList* stack){
	struct tokenizeList* sstack = stack;
	stack = stack -> next;
	free(sstack);
	return stack;
}
/////////////////////////////////////////////////Initialize ///////////////////////////////////////////////////////

struct lispList* initializeFreeList(){
	for(int i = 0; i < heapSize ; i ++){
		heap[i].next = heap + i + 1;
		heap[i].useful = 0;
	}
	heap[heapSize - 1].next = NULL;
	
	return heap;
}

void initializeCommandList(){
	commandList[0] = EVAL;
	commandListIndex = 1;
}
/*
Функция initializeFreeList() инициализирует список свободных ячеек
памяти из массива heap, который выступает в роли хранилища элементов 
типа lispList. Она заполняет next каждой ячейки так, чтобы они указывали 
на следующую свободную ячейку памяти. Возвращаемое значение - 
указатель на первый элемент массива.
*/
/////////////////////////////////////////////////Print//////////////////////////////////////////////////////////



void printString(struct lispList* listString);

void printElement(union element info, enum type t){ 
	if( t == INTEGER){
		printf("%d ", info.num );
	}else if(t == LIST){
		printf("( ");
		printList( info.list );
		printf(") ");
	}else if(t == SYMBOL){
		printf("'%c'", info.symbol);
	}else if(t == STRING){
		printString( info.list );
	}
	else{
		printError("Wrong type", __FUNCTION__);
	}	
	
}


void printList(struct lispList* list){
	for(; list != NULL; list = list->next){
		printElement(list->info, list->t);
	}
			
}

void printTokenizeList(struct tokenizeList* list){
	for(; list != NULL; list = list->next){
		printf("'%s' ", list->info);
	}
	printf("\n");
}

void printString(struct lispList* listString){
	printf("%s",_YELLOWTEXT_);
	for(; listString != NULL; listString = listString->next){
		printf("%c", listString->info.symbol);
	}
	printf("%s ", _NORMALTEXT_);

}


void printEvalData(struct evalData element){
	if(element.t == INTEGER){
		printf("%d ", element.info.num );
	}else if(element.t == SYMBOL){
		printf("%c ", element.info.symbol );
	}
	else if(element.t == LIST){
		printf("( ");
		printList(element.info.list);
		printf(") ");
	}else if(element.t == STRING){
		printString(element.info.list);
	}
	else{
		printf("type %d\n", element.t);
		printError("Wrong type for this structure", __FUNCTION__);
	}
}
void printCommand(enum commands comand){
	if(comand == APPLY){
		printf("APPLY ");
	}else if(comand == EVAL){
		printf("EVAL ");
	}else if(comand == CONS){
		printf("CONS ");
	}
	else{
		printError("wrong type", __FUNCTION__);
	}
}
/*
Функция printList() выводит на экран значения всех элементов типа
lispList списка, переданного в качестве параметра. Она проходит по 
всем элементам, вызывая функцию printElement() для каждого из них.
*/

void printEvalStack(){
	printf("evalStack\n[ ");
	for(int i = 0; i < stackEvalIndex; i++){
		printEvalData(stackEval[i]);
	}
	printf("]\nstackEvalIndex = %d\n\n", stackEvalIndex);
}

void printEvaledStack(){
	printf("evaledStack\n[ ");
	for(int i = 0; i < evaledIndex; i++){
		printEvalData(evaled[i]);
	}
	printf("]\nevaledIndex = %d\n\n", evaledIndex);
}

void printcommandsList(){
	printf("evaledStack\n[ ");
	for(int i = 0; i < commandListIndex; i++){
		printCommand(commandList[i]);
	}
	printf("]\ncommandListIndex = %d\n\n", commandListIndex);
	
}


void printError(const char* text, const char* functionName){
	printf("\n%sError:%s '%s' in function %s%s\n\n%s",_REDTEXT_,_NORMALTEXT_, text, _GREENTEXT_,functionName,_NORMALTEXT_);
	exit(1);
	
}

/////////////////////////////////////////////////Create Structure////////////////////////////////////////////////////////////


struct evalData createEvalDataNum(int num){
	struct evalData res;
	res.info.num = num;
	res.t = INTEGER;
	return res;
}
struct evalData createEvalDataList(struct lispList* list){
	struct evalData res;
	res.info.list = list;
	res.t = LIST;
	return res;
}

struct evalData createEvalDataUn(union element info, enum type t){
	struct evalData res;
	if(t == INTEGER){
		res.info.num = info.num;
	}
	else if(t == SYMBOL){
		res.info.symbol = info.symbol;
	}		
	else if(t == LIST){
		res.info.list = info.list;
	}
	else if(t == STRING){
		res.info.list = info.list;
	}
	else{
		printError("wrong type", __FUNCTION__);
	}
	res.t = t;
	return res;	
}
///////////////////////////////////////////////Conversion///////////////////////////////////////////////////////////
struct evalData convertLispListToEvalData( struct lispList* list){
	return createEvalDataUn(list->info, list->t);
}

struct evalData convIntToEvalData(int num){
	return	createEvalDataNum( num);
}

///////////////////////////////////////////////Mark And Sweep//////////////////////////////////////////////////////////
void mark(struct lispList* list ){
	if(list != NULL && list->useful != 1){
		if(list->t == LIST){
			list->useful = 1;
			mark(list->info.list);
		}else if(list->t == STRING){
			list->useful = 1;
			mark(list->info.list);
		}
		else{
			list->useful = 1;
		} 
	mark(list->next);
	}
}
	
void sweep(){
    struct lispList* res = NULL;
    for(int i = 0; i < heapSize ; i ++){
	if(heap[i].useful){
	    heap[i].useful = 0;
	}
	else{
	    heap[i].next = res;
	    res = heap + i;
	}
    }
    freeList = res;
    
}

void markAndSweep(struct lispList* list ){
    mark(list);
    sweep();
}

/////////////////////////////////////////////////Cons Elemnt//////////////////////////////////////////////////////////

struct lispList* consInteger( int num, struct lispList* pointer){ 	   // Cоздает ячейку типа INTEGER
	struct lispList* cell = freeList;
	freeList = freeList->next;
		
	cell->info.num = num;
	cell->t = INTEGER;
	//cell->refCount ++;
	cell->next = pointer;
	
	return cell;	
}

struct lispList* consSymbol( char symbol, struct lispList* pointer){ 	   // Cоздает ячейку типа INTEGER
	struct lispList* cell = freeList;
	freeList = freeList->next;
		
	cell->info.symbol = symbol;
	cell->t = SYMBOL;
	//cell->refCount ++;
	cell->next = pointer;
	
	return cell;	
}

struct lispList* consList( struct lispList* info, struct lispList* pointer){// Cоздает ячейку типа LIST
	struct lispList* cell = freeList;
	freeList = freeList->next;
		
	cell->info.list = info;
	//if(info != NULL){
	//	info->refCount ++;
	//}
	//cell->refCount ++;
	cell->t = LIST;
	cell->next = pointer;
	
	return cell;	
}

struct lispList* consString( struct lispList* info, struct lispList* pointer){// Cоздает ячейку типа LIST
	struct lispList* cell = freeList;
	freeList = freeList->next;
		
	cell->info.list = info;
	/*if(info != NULL){
		info->refCount ++;
	}
	cell->refCount ++;*/
	cell->t = STRING;
	cell->next = pointer;
	
	return cell;	
}



/*
Функции consInteger() и consList() создают новую ячейку памяти, заполняют
ее переданными параметрами и возвращают указатель на нее. 
*/
/////////////////////////////////////////////////Add Elemnt/////////////////////////////////////////////////////////
void addToStack(struct evalData data){
	stackEval[stackEvalIndex++] = data;
	
}
void addToEvaled(struct evalData data){
	evaled[evaledIndex++] = data;
}
void addToCommandList(enum commands data){
	commandList[commandListIndex++] = data;
}
/////////////////////////////////////////////////delete Elemnt////////////////////////////////////////////////////////////
struct evalData deleteFromStack(){
	return stackEval[stackEvalIndex--];
}
struct evalData deleteFromEvaled(){
	return evaled[evaledIndex--];
}
enum commands deleteFromCommandList(){
	return commandList[commandListIndex--] ;
}
/////////////////////////////////////////////////Get Elemnt////////////////////////////////////////////////////////////

struct evalData getStackEval(){
	return stackEval[stackEvalIndex - 1];
}
struct evalData getEvaled(){
	return evaled[evaledIndex - 1];
}
enum commands getCommandList(){
	return commandList[commandListIndex - 1] ;
}

/////////////////////////////////////////////////Get Index////////////////////////////////////////////////////////////
int getStackEvalIndex(){
	return stackEvalIndex - 1;
}
int getEvaledIndex(){
	return evaledIndex - 1;
}
int getCommandListIndex(){
	return commandListIndex - 1;
}

/////////////////////////////////////////////////Strings////////////////////////////////////////////////////////////
int listLen( struct lispList* list);


int isNumber(char* string){ // Функция isNumber() проверяет является ли строка числом 
	int stringLen = strlen(string);
	int i = 0;
	if(stringLen != 1 && string[0] == '-'){
		i = 1;
	}
	for(; i < stringLen; i++){
		if(string[i] < '0' || string[i] > '9')
			return 0;
	}
	return 1;
}

int strToInt(char *string){
	if (*string == '\0' ){
		return 0;
	}
	char *endptr;
	int num = strtol(string, &endptr, 10);
	return num;
}
/*Функция slicesStr() вырезает из строки символы с 
аданным диапазоном индексов и возвращает полученную подстроку.*/

char* slicesStr (char* str, int start, int end){
	if(start > end){
		exit(1);
	}
	int len = end - start;
	
	char* res = malloc(sizeof(char) * (len+1));
	for(int g = 0; start < end; start++, g++){
		res[g] = str[start];
	}
	res[len] ='\0';

	return res;
} 


char* charToString (char symbol){ //Функция charToString() преобразует символ в строку.
  	 char* res = malloc(sizeof(char)*2);
  	 res[0] = symbol;
  	 res[1] = '\0';
     return res;	
}

int eqStrings(char* string1, char* string2){
	return !strcmp(string1, string2);
}





struct lispList* stringToList(char* string){
	struct lispList* res = NULL;
	int stringLen = strlen(string);
	for(int i = stringLen; i >= 0; i --){
		res = consSymbol(string[i], res);
	}
	return res;
		
}
char* charListToString(struct lispList* listString){
	int len  = listLen(listString);
	char* res = malloc(sizeof(char)*(len + 1));
	for(int i = 0; i < len; i ++, listString = listString->next){
		res[i] = listString->info.symbol;
	}
	res[len] ='\0';
	return res;
	
}

int eqListString(struct lispList* list1, struct lispList* list2){
	for(;list1 != NULL && list2 != NULL; list1 = list1->next, list2 = list2 ->next){
		if(list1->info.symbol != list2->info.symbol){
			return 0;
		}
	}
	return list1 == list2; 
}
//////////////////////////////////////////////////////List////////////////////////////////////////////////////////////

struct lispList* reverseLispList(struct lispList* list){ //Функция reverseLispList() изменяет порядок элементов списка на обратный.
	struct lispList* res = NULL;
	for(; list != NULL; ){
		struct lispList* listSave = list->next;
	 	list->next = res; 
	 	res = list;
	 	list = listSave;
	}
	return res;	
}


struct tokenizeList* reverseTokenizeList(struct tokenizeList* list){ // Функция reverseTokenizeList() изменяет порядок элементов списка на обратный.
	struct tokenizeList* res = NULL;
	for(; list != NULL; ){
		struct tokenizeList* listSave = list->next;
	 	list->next = res; 
	 	res = list;
	 	list = listSave;
	}
	return res;
}

struct lispList* cutOutList(struct lispList* startPoint, struct lispList*  endPoint){ // обе переменные хронят в себе элементы которые не должны не находится внутри нового листа
	struct lispList* resList = startPoint;
	struct lispList* searchElement = startPoint;
	if(startPoint == endPoint){
		searchElement->next = NULL;
		return NULL;
	}
	while(searchElement->next != endPoint){ //->next
		searchElement = searchElement->next;
	}
	searchElement->next = NULL;
	return resList;
}



int listLenDeep( struct lispList* list){
	if(list == NULL){
		return 0;
	}
	else if(list->t == LIST){
		return listLenDeep(list->info.list) + listLenDeep(list->next) + 1;	
	}
	else if(list->t == STRING){
		return listLenDeep(list->info.list) + listLenDeep(list->next) + 1;	
	}
	else{
		return listLenDeep(list->next) + 1;
	}
	
}


int listLen( struct lispList* list){
	int len = 0;
	for(; list != NULL; list = list->next, len ++ );
	return 	len;
}





/////////////////////////////////////////////////Tokenize//////////////////////////////////////////////////////////


struct tokenizeList*  addString (char* data, struct tokenizeList* ls){ //Функция addString() создает новый элемент типа tokenizeList.

	struct tokenizeList* res = malloc( sizeof(struct tokenizeList)); 
	res->info = data;
	res->next = ls;
	return res;
}


struct tokenizeList* reverseTokenizeList(struct tokenizeList* list); // Функция reverseTokenizeList() изменяет порядок элементов списка на обратный.


/*Функция slicesStr() вырезает из строки символы с 
аданным диапазоном индексов и возвращает полученную подстроку.*/

char* slicesStr (char* str, int start, int end);


//Функция charToString() преобразует символ в строку.
char* charToString (char symbol); 



/*Функция printTokenizeList() выводит на экран значения 
всех элементов типа tokenizeList переданного списка.*/

void printTokenizeList(struct tokenizeList* list);



void freeTokenizeList(struct tokenizeList* list){ //Функция freeTokenizeList() очищает все елементв списка типа tokenizeList
	struct tokenizeList* listSave;
	while( list != NULL){
		listSave = list;
		list = list->next;
		free(listSave->info);
		free(listSave);
	}
}



struct tokenizeList*  tokenize(char* str){
	int len = strlen(str);
	int start = 0;
	int i = 0;
	int addHappened = 1; 
	struct tokenizeList* ls = NULL;
	
	for(; i < len; i ++){
		if((str[i] == ' '|| str[i] == '\n' || str[i] == '\t') && !addHappened){ // если жто пустая клетка и добапления не произошло 
			ls = addString(slicesStr(str, start, i), ls); //вырезание нужной информации в результат 
			start = i + 1 ;
			addHappened = 1;
		}
		else if (( str[i] == '(' || str[i] == ')')){ // нахождение скобок
		    if (!addHappened){
		    	ls = addString(slicesStr(str, start, i), ls);  //вырезание предыдущей информации в результат 
		    }
			ls = addString(charToString(str[i]), ls);
			start = i + 1;
			addHappened = 1;
		}
		else if ((str[i] == ' '|| str[i] == '\n' || str[i] == '\t') && addHappened){//пропуск лишних пробелов  
			start = i + 1;
		}
		else{
			addHappened = 0;
		}
	} 
	if(i != start){
		ls = addString(slicesStr(str, start, i), ls );
	}
	//free(str);
	return reverseTokenizeList(ls); //reverslist(reverselist_copy
} 



/*
Функции printTokenizeList(), addString(), reverseTokenizeList(), slicesStr()
и charToString() отвечают за токенизацию входной строки. 
*/
/////////////////////////////////////////////////Parenthesis Check//////////////////////////////////////////////////////////

int parenthesisCheck(struct tokenizeList* list){
	int leftParenthesis = 0;
	int rightParenthesis = 0; 
	for(; list != NULL && leftParenthesis >= rightParenthesis; list = list->next){
		if(eqStrings(list->info, "(")){
			leftParenthesis += 1;
		}
		else if(eqStrings(list->info, ")")){
			rightParenthesis += 1;
		}
	}	
	return (leftParenthesis == rightParenthesis);
}




/////////////////////////////////////////////////Define Type///////////////////////////////////////////////////////
int isNumber(char* string);


struct lispList* defineType(char* info,struct lispList* stack){
		if(isNumber(info)){
			int num = strToInt(info);
			free(info);
			return consInteger(num , stack );
		}
		else if( eqStrings(info,"(")  ){//|| eqStrings(info,")"
			char symbol = info[0];
			free(info);
			//struct lispList* res = consSymbol(symbol, stack );
			return consSymbol(symbol, stack );
		}
		else{
			struct lispList* charList =  stringToList(info);
			free(info);
			return consString(charList, stack);
			//printError("Type cannot be defined", __FUNCTION__);
		}
		
}




/////////////////////////////////////////////////Parsing///////////////////////////////////////////////////////

struct lispList* parsing (struct tokenizeList* list){

	struct lispList* stack = NULL;
	
	while (list != NULL){
		char* elemnt = list->info;
		if( !eqStrings(elemnt, ")")){ // добавление чисел и строк на стек 
			stack = defineType(elemnt, stack);
		} 
		else if( eqStrings(elemnt, ")")){  //добавление списков на стек
			
			free(list->info);
			struct lispList* res;
			
			if ( stack->t == SYMBOL && stack->info.symbol == '('){
				
				stack = stack->next;
				res = NULL;		
			} 
			else{
				struct lispList* listStart = stack;
	           	struct lispList* listEnd = stack; 
				
				while (  listEnd->t != SYMBOL )  {
					listEnd = listEnd->next;
				}
				res = cutOutList(listStart, listEnd); 
				stack = listEnd->next;
				
			}
			stack = consList( reverseLispList(res) ,stack);
		
		}
		list = freeFirstStackShallow(list);
		
	}
	return reverseLispList(stack);
}

/////////////////////////////////////////////////Eval //////////////////////////////////////////////////////////
struct evalData extractData();

struct evalData eval(struct lispList* listBase){
	struct lispList* list = listBase->info.list;
	if (list->t != STRING){
		printError("First element is not a function", __FUNCTION__);
	} 
	if(eqListString(list->info.list, stringToList("+"))){
		struct lispList* num1Pointer = list->next;
		struct lispList* num2Pointer = list->next->next;
		struct evalData num1 = extractData(num1Pointer); 
		struct evalData num2 = extractData(num2Pointer); 
		return createEvalDataNum(num1.info.num + num2.info.num );	
	}else if(eqListString(list->info.list, stringToList("-"))){
		struct lispList* num1Pointer = list->next;
		struct lispList* num2Pointer = list->next->next;
		struct evalData num1 = extractData(num1Pointer); 
		struct evalData num2 = extractData(num2Pointer); 
		return createEvalDataNum(num1.info.num - num2.info.num );
	}else if(eqListString(list->info.list, stringToList("*"))){
		struct lispList* num1Pointer = list->next;
		struct lispList* num2Pointer = list->next->next;
		struct evalData num1 = extractData(num1Pointer); 
		struct evalData num2 = extractData(num2Pointer); 
		return createEvalDataNum(num1.info.num * num2.info.num );
		
	}else if(eqListString(list->info.list, stringToList("/"))){
		struct lispList* num1Pointer = list->next;
		struct lispList* num2Pointer = list->next->next;
		struct evalData num1 = extractData(num1Pointer); 
		struct evalData num2 = extractData(num2Pointer); 
		return createEvalDataNum(num1.info.num / num2.info.num );	
	}else if(eqListString(list->info.list, stringToList("quote"))){
		struct lispList* pointer = list->next;
		return createEvalDataUn(pointer->info, pointer->t);
	}
	else{
		printError("Invalid function", __FUNCTION__);
	}	
}


void putCommands(){
	if(getCommandList() == EVAL){
		deleteFromCommandList();
		if(getStackEval().t == LIST){
			if(1){//( eqListString(getStackEval().info.list, stringToList("+"))){
				printString(getStackEval().info.list);
				printf("\n");
				addToEvaled(convertLispListToEvalData(getStackEval().info.list));
				addToCommandList(EVAL);
				addToCommandList(EVAL);
				addToCommandList(APPLY2) ; 
				struct lispList* firstNumber = getStackEval().info.list->next;
				struct lispList* secondtNumber = getStackEval().info.list->next->next;
				deleteFromStack();
				addToStack(convertLispListToEvalData(firstNumber));
				addToStack(convertLispListToEvalData(secondtNumber));
				//(+ 1 2) -> 1 2
			}
			else{
				printError("wrong operation", __FUNCTION__);
			}
		}
		else if(getStackEval().t == INTEGER){
			addToEvaled(getStackEval());
			deleteFromStack();
		}
		else{
			printError("wrong type", __FUNCTION__);
		}
	}else if(getCommandList() == APPLY2){
		struct evalData secondInfo = 	deleteFromEvaled();
		struct evalData firstInfo = 	deleteFromEvaled();
		struct evalData operation = 	deleteFromEvaled();
		if(eqListString(operation.info.list, stringToList("+"))){
			addToEvaled(convIntToEvalData(firstInfo.info.num + secondInfo.info.num));
		}
	}
	else{
		printError("not a command", __FUNCTION__);
	}
	

}
/*
void evalNew(struct lispList* list){//struct evalData
	//putEvalData(list);
	//while(commandListIndex >= 0){
	for(; commandListIndex >= 0; commandListIndex--){
		if(commandList[commandListIndex] == EVAL){
			if(stackEval[stackEvalIndex].t == LIST){
				printf(" list - ");
				printEvalData(stackEval[stackEvalIndex]);
				printf("\n");
				putEvalData(stackEval->info.list);
			}
			else{
				addToEvaled(stackEval[stackEvalIndex]);
				//putEvaledList(stackEval[stackEvalIndex]);
			}
		}
		
		else if(commandList[commandListIndex] == APPLY){
			printf("APPLY\n\n");
		}
		else{
			printError("worng comand",__FUNCTION__);
		}
	}
	
	//return extractData(listBase);
}*/
//printf("stackEvalIndex = %d\n",stackEvalIndex);
//printEvalData(stackEval[stackEvalIndex]);
//printf("\n");
/*

(* 1 2)
(2 1 '*')



stack: (* ( + 1 2 ) 4)
evaled:
comands: EVAL APLLY

stack:  ( + 1 2 ) 4
evaled: *
comands: EVAL EVAL APPLY

stack:   1 2  4
evaled: + * 
comands: EVAL EVAL APPLY EVAL APPLY

stack:  2  4
evaled: 1 + * 
comands: EVAL APPLY EVAL APPLY

stack:   1 2  4
evaled: 2 1 + * 
comands:  APPLY EVAL APPLY

stack:   4
evaled:  3 * 
comands: EVAL APPLY

stack:   
evaled:  4 3 * 
comands: APPLY

stack:   
evaled:  12 
comands: 

*/

/*
(1 a b)
(3)
(car (quote (a b c))) -> a
(cdr (quote (a b c))) -> (b c)
(cdr '(a b c))
car,cdr, list, if,null, cons


*/


struct evalData extractData(struct lispList* list){
	if(list->t == LIST){
		return eval(list);
	}
	else{
		return createEvalDataNum(list->info.num); 
	}
}


/////////////////////////////////////////////////Main//////////////////////////////////////////////////////////

int main(){
	heap = malloc(sizeof(struct lispList) * heapSize);
	freeList = initializeFreeList();
	initializeCommandList();
	//comandsList[0] = APPLY;
	struct tokenizeList* stringList =  tokenize("  ( + 1 2 )    ");//  ( 1 ( 2 ) ( 1 ) 3 4 )
 	//x = (+ 2 3) =>  5 
	//printTokenizeList(stringList);
	if(!parenthesisCheck(stringList)){
		printError("amount of parenthesis doesn't match",__FUNCTION__ );
	}

	
	//struct lispList* res = parsing(stringList);
	res = parsing(stringList);
	//printf("1\n");
	//evalNew(res);
	addToStack(convertLispListToEvalData(res));
	putCommands();
	//putEvalData(res);
	printEvalStack();
	printEvaledStack();
	
	return 0;
	printList(res);
	printf("\n\n");

	printf("eval res = ");
	printEvalData(eval(res));
	printf("\n\n");
	
	//delZeroReference();
	markAndSweep(res);

	
	printList(res);
	printf("\n");


	
	printf("\n\n%d - %d = %d \n", heapSize - listLen(freeList), listLenDeep(res),  
	heapSize - listLen(freeList) - listLenDeep(res) ); 
	free(heap);
	return 0;


	
	/*printf(" - res\n");
	printf("len res = %d\n", listLenDeep(res));
	printf(" %d - %d = %d (used cells)\n", heapSize, listLen(freeList), heapSize - listLen(freeList) );
	//freeTokenizeList(stringList);
	return 0;
	*/
}

