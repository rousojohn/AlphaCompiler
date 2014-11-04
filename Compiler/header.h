#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define COMMENTS		46
#define INVALID_TOKEN	47
#define ERROR			48

#define BUCKETS 100


extern FILE *yyin ;
extern int yylineno;
extern char * yytext;




/*
#######################
STACK (START)
#######################
*/

typedef struct entryde
{
	int number;
	struct entryde *next;
	struct entryde *previous;
}stackNode;


typedef struct st
{
	stackNode *head;
	stackNode *tail;
	int length;
	
}stack1;



typedef stack1 *Stack;

Stack newStack();

void push(Stack s, int number);
int pop(Stack s);
int top(Stack s);
int isEmpty(Stack s);
int getLength(Stack s);
void printStack(Stack s);
Stack merge(Stack a, Stack b);
/*
=========
PRADEIGMATA LEITOURGEIAS STOIVAS
=========


Stack mystack = newStack();

push(mystack, 5);

int number = pop(mystack);

int number = top(mystack);

if(isEmpty(mystack))
	
	int length = getLength(mystack);

printStack()

*/

/*
#######################
STACK (END)
#######################
*/


typedef struct Variable
{
	const char *name;
	unsigned int scope;
	unsigned int line;
	
}Variable;


typedef struct Function 
{
	const char *name;
	unsigned int scope;
	unsigned int line;
	
}Function;


enum SymbolTableType
{ 
	global, local, formal, userfunc, libfunc
};


enum iopcode{
	assign, add,  sub,   mul,   Div,  mod,  uminus,  and,  or,  not,
	if_eq,if_noteq,if_lesseq,if_greateq,if_less,if_greater, jump,
	call, param,funcstart,funcend,
	tablecreate,tablegetelem,tablesetelem, ret,getretval
	
};

enum expr_t{
	var_e,
	tableitem_e,
	programfunc_e,
	libraryfunc_e,
	arithmexpr_e,
	booleanexpr_e,
	assignexpr_e,
	newtable_e,
	constnum_e,
	constbool_e,
	constchar_e,
	nil_e
};

typedef enum expr_t expr_t;



typedef struct expr expr1;
typedef enum iopcode iopcode;


typedef struct quad{
	iopcode op;
	expr1* arg1;
	expr1* arg2;
	expr1* result;
	 int label;
	 int line;
	 int taddress;
}quad;



enum scopespace_t{
	
	programVar , functionLocal , formalArg
};

enum symbol_t{
	
	var_s , programfunc_s , libraryfunc_s
};


typedef struct SymbolTableEntry
{
	
	int isActive;
	int isVariable;
	
	union
	{
		Variable *varVal;
		Function *funcVal;
	}value;
	unsigned int totalLocals;
	unsigned int offset;
	unsigned int iaddress;
	enum scopespace_t    space;
	enum symbol_t   	type_t;
	enum SymbolTableType type;
	Stack returnList;
	struct SymbolTableEntry *next;
	struct SymbolTableEntry *up;
	struct SymbolTableEntry *next1;
	
}SymbolTableEntry;



unsigned int programVarOffset;
unsigned int functionLocalOffset;
unsigned int formalArgOffset;
unsigned int scopeSpaceCounter;


struct expr{
	expr_t	type;
	SymbolTableEntry *	sym;
	struct expr* 	index;
	double		numConst;
	char*		strConst;
	unsigned int	boolConst;
	unsigned int	 totalLocals;
	struct expr* 	next;
	struct expr*   previous;
	Stack trueList;
	Stack falseList;
	char *name;
	unsigned int method;
	struct expr* next1;
	
};


typedef struct stmt
{
	Stack breakList;
	Stack contList;
	int enter, test;
}stmt;




enum vmopcode{
	
	assign_v, add_v, sub_v, mul_v, Div_v, mod_v, uminus_v , and_v, or_v , not_v ,

	ieq_v, ine_v , jle_v,  jge_v , jlt_v , jgt_v, jump_v ,
	
	call_v , pusharg_v , funcenter_v , funcexit_v ,

	newtable_v , tablegetelem_v , tablesetelem_v , nop_v
	
};



enum vmarg_t{

	label_a  = 0,
	
	global_a = 1,
	
	formal_a = 2,
		
	local_a = 3,
	
	number_a = 4,
	
	string_a = 5,
	
	bool_a = 6,
	
	nil_a = 7,
	
	userfunc_a = 8,
	
	libfunc_a = 9,
	
	retval_a = 10
};


typedef struct vmarg
{
	enum vmarg_t type;
	unsigned val;

}vmarg;


typedef struct instruction
{
	enum vmopcode opcode;
	vmarg *result;
	vmarg *arg1;
	vmarg *arg2;
	unsigned srcLine;
	
}instruction;


instruction *instructions;

typedef struct userfunc
{
	unsigned address;
	unsigned localsize;
	char *id;
	
}userfunc1;


typedef struct incomplete_jump
{
	unsigned instrNo;
	unsigned iaddress;
	struct incomplete_jump *next;
	
}incomplete_jump;

void add_incomplete_jump(unsigned instrNo, unsigned iaddress);

incomplete_jump* ij_head;
unsigned ij_total;

double *numConsts;
unsigned totalNumConsts;

char **stringConsts;
unsigned totalStringConsts;

char **namedLibFuncs;
unsigned totalNamedLibFuncs;

userfunc1 *userfuncs;
unsigned totalUserFuncs;

userfunc1 *userVars;
unsigned totalUserVars;
unsigned uservars_newvar(SymbolTableEntry *sym);

void generate_ADD(quad *);
void generate_SUB(quad *);
void generate_MUL(quad *);
void generate_DIV(quad *);
void generate_MOD(quad *);
void generate_NEWTABLE(quad *);
void generate_TABLEGETELEM(quad *);
void generate_TABLESETELEM(quad *);
void generate_ASSIGN(quad *);
void generate_NOP(quad *);
void generate_JUMP(quad *);
void generate_IF_EQ(quad *);
void generate_IF_NOTEQ(quad *);
void generate_IF_GREATER(quad *);
void generate_IF_GREATEREQ(quad *);
void generate_IF_LESS(quad *);
void generate_IF_LESSEQ(quad *);
void generate_NOT(quad *);
void generate_OR(quad *);
void generate_PARAM(quad *);
void generate_CALL(quad *);
void generate_GETRETVAL(quad *);
void generate_FUNCSTART(quad *);
void generate_RETURN(quad *);
void generate_FUNCEND(quad *);
void generate_UMINUS(quad *q);
void generate_AND(quad *q);



typedef void (*generator_func_t)(quad*);

void generateICodes(void);

void generate(iopcode op, quad *q);
void emit_icode( instruction t);

void make_retvaloperand(vmarg *arg);
void make_booloperand(vmarg *arg, unsigned val);
void make_numberoperand(vmarg *arg, double val);
void make_operand(expr1 *e, vmarg *arg);

void init_instruction(instruction *t);



unsigned consts_newstring(char *s);
unsigned consts_newnumber(double n);
unsigned libfuncs_newused(const char *s);
unsigned userfuncs_newfunc(SymbolTableEntry *sym);


void append(Stack s, unsigned int label);

int currproccessedquad(void);

void backpatch_returnList(Stack s, unsigned int label);

void patch_incomplete_jumps();

unsigned int nextinstructionlabel();

void printInstructions(FILE *fp);

void expand_i(void);

quad* quads;

unsigned int total;
unsigned int currQuad;
unsigned int currInstruction;

#define EXPAND_SIZE 1024
#define CURR_SIZE (total*sizeof(quad))
#define NEW_SIZE (EXPAND_SIZE*sizeof(quad)+CURR_SIZE)

unsigned int total_i;

#define EXPAND_SIZE_I 1024
#define CURR_SIZE_I (total_i*sizeof(instruction))
#define NEW_SIZE_I (EXPAND_SIZE*sizeof(instruction)+CURR_SIZE)


expr1* lvalue_expr(SymbolTableEntry *sym);

expr1* asignexpr_expr();

void init_symbolTable();

void insert_to_table(SymbolTableEntry *entry );

void print_table();

SymbolTableEntry *search_variable_local( const char *name );


void expand(void);
void emit( iopcode op, 
		expr1* arg1,
		expr1* arg2,
		expr1* result,
		 int label,
		 int line
		);
enum scopespace_t currscopespace(void);
unsigned int currscopeoffset(void);
void enterscopespace(void);
void exitscopespace(void);
void inccurrscopeoffset(void);




int Scope; // initialized at SymTable.c -> main
int counterFuncName; // initialized at SymTable.c -> main
int counterTempVar;

struct alpha_list
{
	SymbolTableEntry *head;
};

typedef struct alpha_list *     symbolList;



expr1 *expr_op(expr_t type);




void backpatch(Stack s, int label);


/*

#### Functions ####

SymbolTableEntry *newEntry(enum SymbolTableType type, int isVariable, const char *name, unsigned int scope, unsigned int line );

symbolList newSymbolList();

void insert_to_list(SymbolTableEntry **listHead, SymbolTableEntry *entry, int moveUP);

SymbolTableEntry *lookUp_list(SymbolTableEntry *listHead, SymbolTableEntry *entry, int moveUP);

SymbolTableEntry *get_scope_head(symbolList list, int scope );

void print_list(SymbolTableEntry *listHead);

*/


/*

Example :

isVariable = 1   for Variable

isVariable = 0   for Function


->  SymbolTableEntry *entry = newEntry(..);
*/

SymbolTableEntry *newEntry(enum SymbolTableType type, int isVariable, const char *name, unsigned int scope, unsigned int line, unsigned int offset, enum scopespace_t space, enum symbol_t type_t );



char * getType(enum SymbolTableType type);


void restorecurrscopeoffset(int number);

expr1 *getExprConstInt(int n);
expr1 *getExprConstReal(double n);
void checkminus(expr1 *e);

expr1 *emit_iftableitem(expr1 *e);
expr1 *getExprConstString(char *s);
expr1 *getExprConstNill();
expr1 *getExprConstBool(char * boolean);
expr1 *member_item(expr1 *lvalue, char *name);
expr1 *make_call(expr1* lvalue, expr1* elist);
void patchlabel( unsigned int quadNo, unsigned int label );

/*

Example :

->  symbolList myList = newSymbolList();

*/
symbolList newSymbolList();


void printQuads();



/*

================= SHMEIWSEIS ===============

1. To entry pou pame na eisagoume DEN PREPEI sti Lista (prepei na exoume kanei lookUp_list() pio prin gia na elegksoume)

2. PREPEI na exoume dhmiourgisei prwta ena Node stin scopeList pou tha exei sto isActive tin timi tou scope. 



gia orizontia list tin kaloume ws:


px        insert_to_list(  &(symbolTable[i]->head) , .. );


gia katheti list tin kaloume ws:


px        insert_to_list(  &(scopeHead->up) , ..  );



*/
void insert_to_list(SymbolTableEntry **listHead, SymbolTableEntry *entry, int moveUP);







/*

================= SHMEIWSEIS ===============

!!! To *listHead einai deiktis sto prwto Node (dld ena SymbolTableEntry) tis listas pou psaxnoyme !!!



gia orizontia list tin kaloume ws:


px        lookUp_list(  symbolTable[i]->head , .. );


gia katheti list tin kaloume ws:


px        loookUp_list(  scopeHead->up , ..  );



Epistrefei deikti pros to Node (dld ena SymbolTableEntry) pou vrike sti lista, diaforetika epistrefei NULL

*/
//SymbolTableEntry *lookUp_list(SymbolTableEntry *listHead, SymbolTableEntry *entry, int moveUP);




/*

Pairnei enan akeraio scope kai afou psaxei stin scopeList
epistrefei ena deikti pros to Node pou exei tin katheti lista 
me ola ta symbola to table me auto to scope.
Diaforetika ean dn vrethei to Node me auto to scope 
(dld den yparxei tetoio scope) epistrefei NULL.

*/
SymbolTableEntry *get_scope_head(symbolList list, int scope );






/*

================= SHMEIWSEIS ===============

!!! To *listHead einai deiktis sto prwto Node (dld ena SymbolTableEntry) tis listas pou psaxnoyme !!!



gia orizontia list tin kaloume ws:


px        print_list(  symbolTable[i]->head , .. );


gia katheti list tin kaloume ws:


px        print_list(  scopeHead->up , ..  );

*/

void print_list(SymbolTableEntry *listHead);


/*

void freeList(symbolList list);

*/



char *getRandomFuncName();
char *getRandomTempVar();

int look_up_only_LibFuncs(const char *name);

int look_up_only_Scope0(const char *name);



/*
Epitrefei : 1 ean yparxei function me onoma name kai einai isActive=1
0 ean den yparxei.

kalei tin synartisi hash() me name
pairnei to index
psaxnei sto symbolTable thesh index
*/
int lookUp_function_global( const char *name );



/*
Epitrefei : 1 ean yparxei variable me onoma name sto idio scope
0 ean den yparxei.
*/
int lookUp_variable_local( const char *name );

void getGoodAligmentStr(const char *s);
void getGoodAligmentNum(const int num);

char *getVmopcodeAsString(enum vmopcode opcode);
/*
Epitrefei : 1 ean yparxei variable me onoma name sta prohgoumena scopes
0 ean den yparxei.

*/
int lookUp_name_global( const char *name );

SymbolTableEntry* newTemp();



/*
============================
StackFunc
=========================
*/



typedef struct entryst
{
	SymbolTableEntry *entry;
	struct entryst *next;
	
}stackNodeSt;


typedef struct stack2
{
	stackNodeSt *head;

}stack2;



typedef stack2 *StackFunc;

StackFunc newStackFunc();

void push_func(StackFunc s, SymbolTableEntry *entry);
SymbolTableEntry *pop_func(StackFunc s);
SymbolTableEntry *top_func(StackFunc s);
int isEmpty_func(StackFunc s);

StackFunc funcstack;
