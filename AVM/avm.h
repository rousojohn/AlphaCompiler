#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

void fetchInstructions(FILE *fp);



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



typedef struct userfunc
{
	unsigned address;
	unsigned localsize;
	char *id;
	
}userfunc1;




typedef struct instruction
{
	enum vmopcode opcode;
	vmarg *result;
	vmarg *arg1;
	vmarg *arg2;
	unsigned srcLine;
	
}instruction;


instruction *instructions;

int totalInstructions;


double *numConsts;
unsigned totalNumConsts;

char **stringConsts;
unsigned totalStringConsts;

char **namedLibFuncs;
unsigned totalNamedLibFuncs;

userfunc1 *userfuncs;
unsigned totalUserFuncs;


// ================================== AVM ===================================


#define AVM_STACKENV_SIZE 4
#define AVM_MAX_INSTRUCTIONS (unsigned) nop_v
#define AVM_ENDING_PC codeSize
#define execute_add execute_arithmetic
#define execute_sub execute_arithmetic
#define execute_mul execute_arithmetic
#define execute_div execute_arithmetic
#define execute_mod execute_arithmetic

#define AVM_NUMACTUALS_OFFSET +4
#define AVM_SAVEDPC_OFFSET +3
#define AVM_SAVEDTOP_OFFSET +2
#define AVM_SAVEDTOPSP_OFFEST +1


#define AVM_STACKSIZE 4096

#define AVM_WIPEOUT(n) memset(&(n), 0, sizeof(n))


#define AVM_TABLE_HASHSIZE 211


#define avm_error printf
#define avm_warning printf


enum memcell_t{
	number_m = 0,
	string_m = 1,
	bool_m = 2,
	table_m = 3,
	userfunc_m = 4,
	libfunc_m = 5,
	nil_m = 6,
	undef_m = 7
};







typedef struct avm_table
{
	unsigned refcounter;
	struct avm_tableBUCKET *strIndexed[AVM_TABLE_HASHSIZE];
	struct avm_tableBUCKET *numIndexed[AVM_TABLE_HASHSIZE];
	unsigned total;
	
}avm_table;





typedef struct avm_memcell{
	enum memcell_t type;
	union{
		double numVal;
		char * strVal;
		unsigned char boolVal;
		avm_table *tableVal;
		unsigned funcVal;
		char *libfuncVal;
	}data;
}avm_memcell;



typedef struct avm_tableBUCKET
{
	avm_memcell key;
	avm_memcell value;
	struct avm_tableBUCKET *next;
	
}avm_tableBUCKET;





avm_memcell avmstack[AVM_STACKSIZE];

avm_memcell *globalVars;
unsigned totalGlobalVars;
void init_global_variables();


avm_memcell ax, bx, cx, retval;



unsigned top, topsp;

double consts_getnumber(unsigned index);
char *consts_getstring(unsigned index);
char *libfuncs_getused(unsigned index);

char * itoa(int num);

unsigned char executionFinished;
unsigned pc;
unsigned currLine;
unsigned codeSize;
instruction *code;
unsigned totalActuals;




typedef void (*execute_func_t)(instruction *);

typedef void (*memclear_func_t)(avm_memcell *);


typedef double (*arithmetic_func_t)(double x, double y);

typedef unsigned char (*tobool_func_t)(avm_memcell *);

typedef void (*library_func_t)(void);


int getLibFuncINDEX(char *id);


extern char *typeStrings [];
extern memclear_func_t memclearFuncs[];
extern tobool_func_t toboolFuncs[];
extern char * (*tostringfuncs[])(avm_memcell *);
extern arithmetic_func_t arithmeticFuncs[];
extern execute_func_t executeFuncs[];
extern library_func_t libfunc[];


void execute_cycle(void);

avm_memcell *avm_translate_operand(vmarg *arg, avm_memcell *reg);


avm_memcell *avm_tablegetelem( avm_table *table, avm_memcell *index ); /*8elei ulopoiisi*/

void avm_tablesetelem( avm_table *table , avm_memcell *index, avm_memcell *content);/*8elei ulopoiisi*/
void avm_registerlibfunc(char *id, library_func_t addr); 
library_func_t avm_getlibraryfunc(char *id);


unsigned avm_totalactuals(void);
avm_memcell *avm_getactual(unsigned i);
void libfunc_print(void);
void libfunc_typeof(void);
void libfunc_totalarguments(void);
void libfunc_input(void);
void libfunc_objectmemberkeys(void);/*8elei ulopoiisi*/
void libfunc_objecttotalmembers(void);/*8elei ulopoiisi*/
void libfunc_objectcopy(void);/*8elei ulopoiisi*/
void libfunc_argument(void);/*8elei ulopoiisi*/
void libfunc_srtonum(void);
void libfunc_sqrt(void);
void libfunc_cos(void);
void libfunc_sin(void);


double add_impl(double x, double y);
double sub_impl(double x, double y);
double mul_impl(double x, double y);
double div_impl(double x, double y);
double mod_impl(double x, double y);

void execute_arithmetic (instruction *instr);

unsigned char number_tobool(avm_memcell *m);
unsigned char string_tobool(avm_memcell *m);
unsigned char bool_tobool(avm_memcell *m);
unsigned char table_tobool(avm_memcell *m);
unsigned char userfunc_tobool(avm_memcell *m);
unsigned char libfunc_tobool(avm_memcell *m);
unsigned char nil_tobool(avm_memcell *m);
unsigned char undef_tobool(avm_memcell *m);
unsigned char avm_tobool(avm_memcell *m);


void execute_assign(instruction *instr);
void execute_add(instruction *instr);
void execute_sub(instruction *instr);
void execute_mul(instruction *instr);
void execute_div(instruction *instr);
void execute_mod(instruction *instr);
void execute_jeq(instruction *instr);
void execute_jne(instruction *instr);
void execute_jle(instruction *instr);
void execute_jge(instruction *instr);
void execute_jlt(instruction *instr);
void execute_jgt(instruction *instr);
void execute_call(instruction *instr);
void execute_pusharg(instruction *instr);
void execute_funcenter(instruction *instr);
void execute_funcexit(instruction *instr);
void execute_newtable(instruction *instr);
void execute_tablegetelem(instruction *instr);
void execute_tablesetelem(instruction *instr);
void execute_nop(instruction *instr);/*8elei ulopoiisi*/


avm_table *avm_tablenew(void);
void avm_tabledestroy(avm_table *t);
void avm_tableincrefcounter(avm_table *t);
void avm_tabledecrefcounter(avm_table *t);
void avm_tablebucketsinit(avm_tableBUCKET **p);
void avm_tablebucketsdestroy(avm_tableBUCKET **p);

void memclear_string(avm_memcell *m);
void memclear_table(avm_memcell *m);
void avm_memcellclear(avm_memcell *m);
//void avm_warning(char *format, ...);
void avm_assign(avm_memcell *lv, avm_memcell *rv);
//void avm_error(char *format, ...);
char *avm_tostring(avm_memcell *);
void avm_calllibfunc(char *funcName);
void avm_callsaveenviroment(void);
void avm_dec_top(void);
void avm_push_envvalue(unsigned val);

userfunc1 *avm_getfuncinfo(unsigned address);
unsigned avm_get_envvalue(unsigned i);

char* number_tostring (avm_memcell *);
char* string_tostring (avm_memcell *);
char* bool_tostring (avm_memcell *);
char* table_tostring (avm_memcell *);/*8elei ulopoiisi*/
char* userfunc_tostring (avm_memcell *);
char* libfunc_tostring (avm_memcell *);
char* nil_tostring (avm_memcell *);
char* undef_tostring (avm_memcell *);

static void avm_initstack(void);

int isNumber(const char *s);