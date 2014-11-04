#include "header.h"

void generateInstructions(void)
{
	FILE *instructionsFile;
	int i;

	int nULL = -1;
	int notNull = -2;

	instructionsFile = fopen("ins.b", "wb+");

	// NUMBERS
	fwrite(&totalNumConsts, sizeof(int), 1, instructionsFile);
	fwrite(numConsts, sizeof(double), totalNumConsts, instructionsFile);


	
	// STRINGS
	fwrite(&totalStringConsts, sizeof(int), 1, instructionsFile);
	for(i=0;i<totalStringConsts;i++)
	{
		int len = strlen(stringConsts[i])+1;
		fwrite(&len, sizeof(int), 1, instructionsFile);
		fwrite(stringConsts[i], sizeof(char), len, instructionsFile);
	}

	

	// LIBRARY FUNCTIONS
	fwrite(&totalNamedLibFuncs, sizeof(int), 1, instructionsFile);
	for(i=0;i<totalNamedLibFuncs;i++)
	{
		int len = strlen(namedLibFuncs[i])+1;
		fwrite(&len, sizeof(int), 1, instructionsFile);
		fwrite(namedLibFuncs[i], sizeof(char), len, instructionsFile);
	}


	// USER FUNCTIONS
	fwrite(&totalUserFuncs, sizeof(int), 1, instructionsFile);
	for(i=0;i<totalUserFuncs;i++)
	{

		fwrite(&userfuncs[i].address, sizeof(unsigned), 1, instructionsFile);
		fwrite(&userfuncs[i].localsize, sizeof(unsigned), 1, instructionsFile);

		int len = strlen(userfuncs[i].id)+1;
		fwrite(&len, sizeof(int), 1, instructionsFile);
		fwrite(userfuncs[i].id, sizeof(char), len, instructionsFile);
		
	}
	


	
	// INSTRUCTIONS
	fwrite(&currInstruction, sizeof(int), 1, instructionsFile);

	for(i=0;i<currInstruction;i++)
	{
		fwrite(&instructions[i].opcode, sizeof(enum vmopcode), 1, instructionsFile);
		
		
		if(instructions[i].result!=NULL)
		{
			fwrite(&notNull, sizeof(int), 1, instructionsFile);
			fwrite(&instructions[i].result->type, sizeof(enum vmarg_t), 1, instructionsFile);
			fwrite(&instructions[i].result->val, sizeof(unsigned), 1, instructionsFile);
		}
		else
			fwrite(&nULL, sizeof(int), 1, instructionsFile);
			


		if(instructions[i].arg1!=NULL)
		{
			fwrite(&notNull, sizeof(int), 1, instructionsFile);
			fwrite(&instructions[i].arg1->type, sizeof(enum vmarg_t), 1, instructionsFile);
			fwrite(&instructions[i].arg1->val, sizeof(unsigned), 1, instructionsFile);
		}
		else
			fwrite(&nULL, sizeof(int), 1, instructionsFile);


		
		if(instructions[i].arg2!=NULL)
		{
			fwrite(&notNull, sizeof(int), 1, instructionsFile);
			fwrite(&instructions[i].arg2->type, sizeof(enum vmarg_t), 1, instructionsFile);
			fwrite(&instructions[i].arg2->val, sizeof(unsigned), 1, instructionsFile);
		}
		else
			fwrite(&nULL, sizeof(int), 1, instructionsFile);

		fwrite(&instructions[i].srcLine, sizeof(unsigned), 1, instructionsFile);
	}

	fclose(instructionsFile);
}


void printType(FILE *fp, vmarg *arg)
{
	switch(arg->type)
	{
		// integer - label
		case label_a :

			fprintf(fp, "%d  ",  arg->val);
			
			break;
			
			
		case retval_a :

			fprintf(fp, "%d  ",  arg->val);

			break;
			
		// constnums
		case number_a :

			fprintf(fp, "%lf  ",  numConsts[arg->val]);
			
			break;
			
		// vars
		case global_a :
		case formal_a :
		case local_a :
			
			fprintf(fp, "%s  ",  userVars[arg->val].id);
			
			break;
			
			
		// userfuncts
		case userfunc_a :
		

			fprintf(fp, "%s  ",  userfuncs[arg->val].id);
			
			break;

			
		case libfunc_a :

			fprintf(fp, "%s  ",  namedLibFuncs[arg->val]);
			break;

		// constString
		case string_a :
			
			fprintf(fp, "%s  ",  stringConsts[arg->val]);
			
			break;
			
		case bool_a :
			
			fprintf(fp, "%s  ",  (arg->val==1)?("true"):("false"));
			
			break;
			
		case nil_a :

			fprintf(fp, "NULL  ");
			
			break;
		
		default :  assert(0);
	}
}


void printInstructions(FILE *fp)
{
	int i;

	for(i=0; i<currInstruction; i++)
	{
		
		fprintf(fp, "%d:    %s  ",  i, getVmopcodeAsString(instructions[i].opcode));

		
		switch(instructions[i].opcode)
		{
			case assign_v:
				


				//fprintf(fp, "%s  ",  userVars[instructions[i].result->val].id);

				printType(fp, instructions[i].result);
				printType(fp, instructions[i].arg1 );
				

				break;
				
				
			case add_v:
			case sub_v:
			case mul_v:
			case Div_v:
			case mod_v:
			case and_v:
			case or_v:	

				//fprintf(fp, "%s  ",  userVars[instructions[i].result->val].id);
				printType(fp, instructions[i].result);
				printType(fp, instructions[i].arg1 );
				printType(fp, instructions[i].arg2 );
				
				break;

			case ieq_v:
			case ine_v:
			case jle_v:
			case jge_v:
			case jlt_v:
			case jgt_v:
			case tablesetelem_v:
			case tablegetelem_v:


				printType(fp, instructions[i].result);
				printType(fp, instructions[i].arg1 );
				printType(fp, instructions[i].arg2 );
				
				
				break;


				
			case jump_v:
				
				printType(fp, instructions[i].result);
				
				break;


				
			case call_v:
			case pusharg_v:
			case funcenter_v:
			case funcexit_v:
			case newtable_v:

				printType(fp, instructions[i].arg1);

				break;

				
			case nop_v:

				fprintf(fp, "NOP\n");
				
				break;

			default : assert(0);
		}
		
		/*
		
		fprintf(fp, "%s  ",  userVars[instructions[i].result->val].id);	
		fprintf(fp, "%s  ", userVars[instructions[i].arg1->val].id);
		fprintf(fp, "%s  ",  userVars[instructions[i].arg2->val].id);
	*/
		
		
		//fprintf(fp, "%d\n", instructions[i].srcLine);
		fprintf(fp, "\n");
	}
	puts("\n\n");
}




char *getVmopcodeAsString(enum vmopcode opcode)
{
	switch(opcode)
	{
		case assign_v:

			return "ASSIGN";

		case add_v:

			return "ADD";

		case sub_v:

			return "SUB";

		case mul_v:

			return "MUL";

		case Div_v:

			return "DIV";
			
		case mod_v:

			return "MOD";
			
		case and_v:

			return "AND";
			
		case or_v:

			return "OR";
		
		case not_v:

			return "NOT";
			
		case ieq_v:

			return "IEQ";
			
		case ine_v:

			return "INE";
			
		case jle_v:

			return "JLE";
			
		case jge_v:

			return "JGE";
			
		case jlt_v:

			return "JLT";

		case jgt_v:

			return "JGT";
			
		case jump_v:

			return "JUMP";
			
		case call_v:

			return "CALL";
			
		case pusharg_v:

			return "PUSHARG";
			
		case funcenter_v:

			return "FUNCENTER";
			
		case funcexit_v:

			return "FUNCEXIT";

		case newtable_v:

			return "NEWTABLE";
			
		case tablegetelem_v:

			return "TABLEGETELEM";
			
		case tablesetelem_v:

			return "TABLESETELEM";
			
		case nop_v:

			return "NOP";

		default : assert(0);
	}
}



void init_instruction(instruction *t)
{
	t->arg1 = malloc(sizeof(vmarg));
	t->arg2 = malloc(sizeof(vmarg));
	t->result = malloc(sizeof(vmarg));
}


unsigned int nextinstructionlabel()
{
	return currInstruction;
}


void add_incomplete_jump(unsigned instrNo, unsigned iaddress)
{
	
	incomplete_jump *tmp = ij_head;

	if(ij_head==NULL)
	{
		ij_head = malloc(sizeof(incomplete_jump));
		ij_head->next = NULL;
		ij_head->instrNo=instrNo;
		ij_head->iaddress=iaddress;
		return;
	}

	while(tmp->next!=NULL)
	{
		tmp = tmp->next;
	}
	tmp->next = malloc(sizeof(incomplete_jump));
	tmp->next->instrNo = instrNo;
	tmp->next->iaddress = iaddress;
	tmp->next->next = NULL;
	
}


/*
assign, add,  sub,   mul,   Div,  mod,  uminus,  and,  or,  not,
if_eq,if_noteq,if_lesseq,if_greateq,if_less,if_greater, jump,
call, param,funcstart,funcend,
tablecreate,tablegetelem,tablesetelem, ret,getretval
*/

generator_func_t generators[] =
{
	generate_ASSIGN,
	generate_ADD,
	generate_SUB,
	generate_MUL,
	generate_DIV,
	generate_MOD,
	generate_UMINUS,

	generate_AND,
	generate_OR,
	generate_NOT,

	generate_IF_EQ,
	generate_IF_NOTEQ,
	generate_IF_LESSEQ,
	generate_IF_GREATEREQ,
	generate_IF_LESS,
	generate_IF_GREATER,
	generate_JUMP,


	generate_CALL,
	generate_PARAM,

	generate_FUNCSTART,
	generate_FUNCEND,

	
	
	generate_NEWTABLE,
	generate_TABLEGETELEM,
	generate_TABLESETELEM,
	generate_RETURN,
	generate_GETRETVAL,
	generate_NOP,

};



void make_operand(expr1 *e, vmarg *arg)
{
	if(e==NULL)
		return;

	switch(e->type)
	{
		case var_e:
		case assignexpr_e:
		case tableitem_e:
		case arithmexpr_e:
		case booleanexpr_e:	
		case newtable_e:
		{
			assert(e->sym);
			//arg->val = e->sym->offset;
			arg->val = uservars_newvar(e->sym);
			
			
			switch(e->sym->space)
			{
				case programVar:
					arg->type = global_a;
					break;

				case functionLocal:
					arg->type = local_a;
					break;

				case formalArg:
					arg->type = formal_a;
					break;

				default : assert(0);
			}
			
			break;
		}
		
		
		case constbool_e:
		{
			arg->val = e->boolConst;
			arg->type = bool_a;
			break;
		}

		case constchar_e:
		{
			arg->val = consts_newstring(e->strConst);
			arg->type = string_a;
			break;
		}

		case constnum_e:
		{
			arg->val = consts_newnumber(e->numConst);
			arg->type = number_a;
			break;
		}

		case nil_e:
		{
			arg->type = nil_a;
			break;
		}
		
		case programfunc_e:
		{
			arg->type = userfunc_a;
			//arg->val = e->sym->iaddress;
			arg->val = userfuncs_newfunc(e->sym);
			break;
		}
		
		case libraryfunc_e:
		{
			arg->type = libfunc_a;
			arg->val = libfuncs_newused(e->sym->value.funcVal->name);
			break;
		}
		
			
		default :  assert(0);

	}

}



void make_numberoperand(vmarg *arg, double val)
{
	arg->val = consts_newnumber(val);
	arg->type = number_a;
}


void make_booloperand(vmarg *arg, unsigned val)
{
	arg->val = val;
	arg->type = bool_a;
}


void make_retvaloperand(vmarg *arg)
{
	arg->type = retval_a;
}





void generateICodes(void)
{
	unsigned int i;
	for(i = 0; i<currQuad; ++i)
		(*generators[quads[i].op])(quads+i);
}


void expand_i(void)
{
	assert(total_i==currInstruction);
	instruction * p = (instruction *) malloc(NEW_SIZE_I);
	if(instructions)
	{
		memcpy(p, instructions, CURR_SIZE_I);
		free(instructions);
	}
	instructions = p;
	total_i += EXPAND_SIZE_I;
}



void emit_icode( instruction t )
{
	if(currInstruction == total_i)
		expand_i();
	
	instruction *p = instructions+currInstruction++;

	p->opcode = t.opcode;
	p->result = t.result;
	p->arg1 = t.arg1;
	p->arg2 = t.arg2;
	p->srcLine = t.srcLine;

}


void generate(iopcode op, quad *q)
{
	instruction t;
	init_instruction(&t);
	
	t.opcode = op; // # WARNING

	t.srcLine = q->line;

	make_operand(q->arg1, t.arg1);
	make_operand(q->arg2, t.arg2);
	make_operand(q->result, t.result);
	q->taddress = nextinstructionlabel();
	emit_icode(t);
}




void generate_relational(iopcode op, quad *q)
{
	instruction t;
	init_instruction(&t);
	
	t.opcode = op; // # WARNING

	t.srcLine = q->line;

	make_operand(q->arg1, t.arg1);
	make_operand(q->arg2, t.arg2);

	t.result->type = label_a;

	if(q->label < currproccessedquad())
	{
		t.result->val = quads[q->label].taddress;
	}
	else
	{
		add_incomplete_jump(nextinstructionlabel(), q->label);
	}
	q->taddress = nextinstructionlabel();
	emit_icode(t);
}




// assign, add,sub,mul,Div,mod,uminus,and,or,not,if_eq,if_noteq,if_lesseq,if_greateq,
// if_less,if_greater,call,param,ret,getretval,funcstart,funcend,
// tablecreate,tablegetelem,tablesetelem,jump

void generate_ADD(quad *q)
{
	generate(add, q);
	
}

void generate_SUB(quad *q)
{
	generate(sub, q);
}


void generate_MUL(quad *q)
{
	generate(mul, q);
}


void generate_DIV(quad *q)
{
	generate(Div, q);
}


void generate_MOD(quad *q)
{
	generate(mod, q);
}


void generate_NEWTABLE(quad *q)
{
	generate(tablecreate, q);
}


void generate_TABLEGETELEM(quad *q)
{
	generate(tablegetelem, q);
}


void generate_TABLESETELEM(quad *q)
{
	generate(tablesetelem, q);
}


void generate_ASSIGN(quad *q)
{
	generate(assign, q);
}


void generate_NOP(quad *q)
{
	instruction t;
	init_instruction(&t);
	t.srcLine = q->line;
	t.opcode = nop_v;
	emit_icode(t);
}


void generate_JUMP(quad *q)
{
	generate_relational(jump, q);
}


void generate_IF_EQ(quad *q)
{
	generate_relational(if_eq, q);
}


void generate_IF_NOTEQ(quad *q)
{
	generate_relational(if_noteq, q);
}


void generate_IF_GREATER(quad *q)
{
	generate_relational(if_greater, q);
}


void generate_IF_GREATEREQ(quad *q)
{
	generate_relational(if_greateq, q);
}


void generate_IF_LESS(quad *q)
{
	generate_relational(if_less, q);
}


void generate_IF_LESSEQ(quad *q)
{
	generate_relational(if_lesseq, q);
}


void generate_NOT(quad *q)
{
	q->taddress = nextinstructionlabel();
	instruction t1,t2,t3,t4;

	init_instruction(&t1);
	init_instruction(&t2);
	init_instruction(&t3);
	init_instruction(&t4);
	
	t1.opcode = ieq_v;
	t1.srcLine = q->line;
	
	make_operand(q->arg1, t1.arg1);
	make_booloperand(t1.arg2, 0);
	
	
	t1.result->type = label_a;
	t1.result->val = nextinstructionlabel()+3;
	emit_icode(t1);


	t2.opcode = assign_v;
	t2.srcLine = q->line;
	make_booloperand(t2.arg1, 0);

	
	make_operand(q->result, t2.result);
	emit_icode(t2);


	t3.opcode = jump_v;
	t3.srcLine = q->line;

	t3.result->type = label_a;
	t3.result->val = nextinstructionlabel()+2;
	emit_icode(t3);

	t4.opcode = assign_v;
	t4.srcLine = q->line;
	
	make_booloperand(t4.arg1, 1);
	make_operand(q->result, t4.result);
	emit_icode(t4);
	
}


void generate_OR(quad *q)
{
	
	q->taddress = nextinstructionlabel();
	instruction t1, t2, t3, t4, t5;
	
	init_instruction(&t1);
	init_instruction(&t2);
	init_instruction(&t3);
	init_instruction(&t4);
	init_instruction(&t5);
	
	t1.srcLine = q->line;
	t1.opcode = ieq_v;
	make_operand(q->arg1, t1.arg1);
	make_booloperand(t1.arg2, 1);
	t1.result->type = label_a;
	t1.result->val = nextinstructionlabel()+4;
	emit_icode(t1);

	t2.opcode = ieq_v;
	t2.srcLine = q->line;
	make_booloperand(t2.arg2, 1);
	make_operand(q->arg2, t2.arg1);
	t2.result->type = label_a;
	t2.result->val = nextinstructionlabel()+3;
	emit_icode(t2);

	t3.opcode = assign_v;
	t3.srcLine = q->line;
	
	make_booloperand(t3.arg1, 0);
	make_operand(q->result, t3.result);
	emit_icode(t3);

	t4.opcode = jump_v;
	t4.srcLine = q->line;
	

	t4.result->type = label_a;
	t4.result->val = nextinstructionlabel()+2;
	emit_icode(t4);

	t5.opcode = assign_v;
	t5.srcLine = q->line;
	make_booloperand(t5.arg1, 1);

	make_operand(q->result, t5.result);
	emit_icode(t5);
	
}

void generate_AND(quad *q)
{
	q->taddress = nextinstructionlabel();
	instruction t1, t2, t3, t4, t5;
	
	init_instruction(&t1);
	init_instruction(&t2);
	init_instruction(&t3);
	init_instruction(&t4);
	init_instruction(&t5);
	
	t1.srcLine = q->line;
	t1.opcode = ieq_v;
	make_operand(q->arg1, t1.arg1);
	make_booloperand(t1.arg2, 0);
	t1.result->type = label_a;
	t1.result->val = nextinstructionlabel()+4;
	emit_icode(t1);
	
	t2.opcode = ieq_v;
	t2.srcLine = q->line;
	make_booloperand(t2.arg2, 0);
	make_operand(q->arg2, t2.arg1);
	t2.result->type = label_a;
	t2.result->val = nextinstructionlabel()+3;
	emit_icode(t2);
	
	t3.opcode = assign_v;
	t3.srcLine = q->line;
	
	make_booloperand(t3.arg1, 1);

	make_operand(q->result, t3.result);
	emit_icode(t3);
	
	t4.opcode = jump_v;
	t4.srcLine = q->line;
	

	t4.result->type = label_a;
	t4.result->val = nextinstructionlabel()+2;
	emit_icode(t4);
	
	t5.opcode = assign_v;
	t5.srcLine = q->line;
	make_booloperand(t5.arg1, 0);
	
	make_operand(q->result, t5.result);
	emit_icode(t5);
	
}




void generate_PARAM(quad *q)
{
	q->taddress = nextinstructionlabel();

	instruction t;
	init_instruction(&t);

	t.srcLine = q->line;
	
	t.opcode = pusharg_v;
	make_operand(q->arg1, t.arg1);
	emit_icode(t);
	
}


void generate_CALL(quad *q)
{
	q->taddress = nextinstructionlabel();
	
	instruction t;
	init_instruction(&t);

	t.srcLine = q->line;
	
	t.opcode = call_v;
	make_operand(q->arg1, t.arg1);
	emit_icode(t);
}


void generate_UMINUS(quad *q)
{
	quad *u = malloc(sizeof(quad));
	u->op = mul;
	u->arg1 = q->arg1;
	u->arg2 = getExprConstInt(-1);
	u->result = q->result;
	u->line = q->line;
	generate_MUL(u);
	
}

void generate_GETRETVAL(quad *q)
{
	q->taddress = nextinstructionlabel();
	instruction t;
	init_instruction(&t);

	t.srcLine = q->line;

	t.opcode = assign_v;
	make_operand(q->arg1, t.arg1);
	make_retvaloperand(t.result);
	emit_icode(t);
}


void generate_FUNCSTART(quad *q)
{
	SymbolTableEntry *f = q->arg1->sym;

	f->iaddress = nextinstructionlabel();
	q->taddress = nextinstructionlabel();

	userfuncs_newfunc(f);
	push_func(funcstack, f);

	instruction t;
	init_instruction(&t);

	t.srcLine = q->line;

	t.opcode = funcenter_v;
	make_operand(q->arg1, t.arg1);
	emit_icode(t);
	
}


void generate_RETURN(quad *q)
{
	q->taddress = nextinstructionlabel();
	instruction t1, t2;
	init_instruction(&t1);
	init_instruction(&t2);
	
	t1.srcLine = q->line;
	t1.opcode = assign_v;
	make_retvaloperand(t1.result);
	make_operand(q->arg1, t1.arg1);
	emit_icode(t1);

	SymbolTableEntry *f = top_func(funcstack);
	append(f->returnList, nextinstructionlabel());

	t2.opcode = jump_v;
	t2.srcLine = q->line;

	t2.result->type = label_a;
	emit_icode(t2);
}


void generate_FUNCEND(quad *q)
{
	SymbolTableEntry *f = pop_func(funcstack);

	backpatch_returnList(f->returnList, nextinstructionlabel());
	q->taddress = nextinstructionlabel();
	instruction t;
	init_instruction(&t);
	t.srcLine = q->line;
	t.opcode = funcexit_v;
	make_operand(q->arg1, t.arg1);
	emit_icode(t);

	
}


unsigned consts_newstring(char *s)
{
	if(stringConsts==NULL)
	{
		stringConsts = malloc(sizeof(char *));
	}
	else
		stringConsts = realloc(stringConsts, (totalStringConsts+1)*sizeof(char *));
	
	stringConsts[totalStringConsts++] = strdup(s);
	return totalStringConsts-1;
}

unsigned consts_newnumber(double n)
{
	if(numConsts==NULL)
	{
		numConsts = malloc(sizeof(double));
	}
	else
		numConsts = realloc(numConsts, (totalNumConsts+1)*sizeof(double));
	
	numConsts[totalNumConsts++] = n;
	return totalNumConsts-1;
}

unsigned libfuncs_newused(const char *s)
{
	if(namedLibFuncs==NULL)
	{
		namedLibFuncs = malloc(sizeof(char *));
	}
	else
		namedLibFuncs = realloc(namedLibFuncs, (totalNamedLibFuncs+1)*sizeof(char *));
	
	namedLibFuncs[totalNamedLibFuncs++] = strdup(s);
	return totalNamedLibFuncs-1;
}


unsigned userfuncs_newfunc(SymbolTableEntry *sym)
{
	if(userfuncs==NULL)
	{
		userfuncs = malloc(sizeof(userfunc1));
	}
	else
		userfuncs = realloc(userfuncs, (totalUserFuncs+1)*sizeof(userfunc1));

	userfuncs[totalUserFuncs].address=sym->iaddress;
	userfuncs[totalUserFuncs].localsize=sym->totalLocals;
	userfuncs[totalUserFuncs].id=strdup(sym->value.funcVal->name);
	totalUserFuncs++;
	return totalUserFuncs-1;
}


unsigned uservars_newvar(SymbolTableEntry *sym)
{
	if(userVars==NULL)
	{
		userVars = malloc(sizeof(userfunc1));
	}
	else
		userVars = realloc(userVars, (totalUserVars+1)*sizeof(userfunc1));
	
	userVars[totalUserVars].address=sym->iaddress;
	userVars[totalUserVars].localsize=sym->totalLocals;
	userVars[totalUserVars].id=strdup(sym->value.varVal->name);
	totalUserVars++;
	return totalUserVars-1;
}


void append(Stack s, unsigned int label)
{
	push(s, label);
}


int currproccessedquad(void)
{
	return currInstruction-1;
}


void patch_incomplete_jumps()
{
	if(ij_head==NULL)
		return;
	
	incomplete_jump *tmpHead = ij_head;
	
	while(tmpHead!=NULL)
	{
		if (tmpHead->iaddress == nextquadlabel() )
		{
			instructions[tmpHead->instrNo].result->type = label_a;
			instructions[tmpHead->instrNo].result->val = currproccessedquad()+1;
		}
		else
		{
			instructions[tmpHead->instrNo].result->type = label_a;
			instructions[tmpHead->instrNo].result->val = quads[tmpHead->iaddress].taddress;
		}
		tmpHead = tmpHead->next;
	}
}

void backpatch_returnList(Stack s, unsigned int label)
{
	if(s==NULL)
		return;
	
	stackNode *tmpHead = s->head;

	while(tmpHead!=NULL)
	{
		tmpHead->number = label;
		tmpHead = tmpHead->next;
	}
}