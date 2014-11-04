#include "avm.h"






library_func_t libfunc[12];



execute_func_t executeFuncs[] = 
{
	execute_assign,
	execute_add,
	execute_sub,
	execute_mul,
	execute_div,
	execute_mod,
	execute_jeq,
	execute_jne,
	execute_jle,
	execute_jge,
	execute_jlt,
	execute_jgt,
	execute_call,
	execute_pusharg,
	execute_funcenter,
	execute_funcexit,
	execute_newtable,
	execute_tablegetelem,
	execute_tablesetelem,
	execute_nop
};



arithmetic_func_t arithmeticFuncs[] = 
{
	add_impl,
	sub_impl,
	mul_impl,
	div_impl,
	mod_impl
};



char * (*tostringfuncs[])(avm_memcell *) = {
	number_tostring,
	string_tostring,
	bool_tostring,
	table_tostring,
	userfunc_tostring,
	libfunc_tostring,
	nil_tostring,
	undef_tostring
};

tobool_func_t toboolFuncs[] = 
{
	number_tobool,
	string_tobool,
	bool_tobool,
	table_tobool,
	userfunc_tobool,
	libfunc_tobool,
	nil_tobool,
	undef_tobool
};


memclear_func_t memclearFuncs[] =
{
	0, // number
	memclear_string,
	0, // bool
	memclear_table,
	0, //userfunc
	0, //libfunc
	0, // nil
	0 // undef
};



char *typeStrings [] = 
{
	"number",
	"string",
	"bool",
	"table",
	"userfunc",
	"libfunc",
	"nil",
	"undef"
};






void execute_cycle()
{
	puts("execute_cycle()");
	
	if(executionFinished)
		return;
	
	else if(pc == AVM_ENDING_PC)
	{
		
		executionFinished = 1;
		return;
	}
	else
	{
		assert(pc<AVM_ENDING_PC);
		
		instruction *instr = instructions+pc;
		
		assert(instr->opcode >=0 && instr->opcode <= AVM_MAX_INSTRUCTIONS);
		
		if(instr->srcLine)
			currLine = instr->srcLine;
		
		unsigned oldPc = pc;

		(*executeFuncs[instr->opcode])(instr);
		
		if(pc == oldPc)
			pc++;
	}
		
}



int getLibFuncINDEX(char *id)
{	
	if(strcmp("print", id)==0)
		return 0;
	
	else if(strcmp("input", id)==0)
		return 1;

	else if(strcmp("objectmemberkeys", id)==0)

		return 2;

	else if(strcmp( "objecttotalmembers" ,id)==0)

		return 3;
	
	else if(strcmp( "objectcopy" ,id)==0)
		
		return 4;
	
	else if(strcmp( "totalarguments" ,id)==0)
		
		return 5;

	else if(strcmp( "argument" ,id)==0)
		
		return 6;

	else if(strcmp( "typeof" ,id)==0)
		
		return 7;

	else if(strcmp( "srtonum" ,id)==0)
		
		return 8;

	else if(strcmp( "sqrt" ,id)==0)
		
		return 9;

	else if(strcmp( "cos" ,id)==0)
		
		return 10;

	else if(strcmp( "sin" ,id)==0)
		
		return 11;

	assert(0);
}



void avm_registerlibfunc(char *id, library_func_t addr)
{
	libfunc[ getLibFuncINDEX(id) ] = addr;
}


library_func_t avm_getlibraryfunc(char *id)
{
	puts("avm_getlibraryfunc()");
	return libfunc[ getLibFuncINDEX(id) ];
}






avm_memcell *avm_translate_operand(vmarg *arg, avm_memcell *reg)
{
	puts("avm_translate_operand()");
	switch(arg->type)
	{
		case global_a:
			
			return &avmstack[AVM_STACKSIZE-1-arg->val];
			
		case local_a:
			
			return &avmstack[topsp-arg->val];
			
		case formal_a:
			
			return &avmstack[topsp+AVM_STACKENV_SIZE+1+arg->val];
		
		case retval_a:
			
			return &retval;
			
		case number_a:
			
			reg->type = number_m;
			reg->data.numVal = consts_getnumber(arg->val);
			return reg;
		
		case string_a:
			
			reg->type = string_m;
			reg->data.strVal = consts_getstring(arg->val);
			return reg;
		
		case bool_a:
			
			reg->type = bool_m;
			reg->data.boolVal = arg->val;
			return reg;
			
		case nil_a:
			
			reg->type = nil_m;
			return reg;
			
		case userfunc_a:
			
			reg->type = userfunc_m;
			reg->data.funcVal = arg->val;
			return reg;
			
		case libfunc_a:
			
			reg->type = libfunc_m;
			reg->data.libfuncVal = libfuncs_getused(arg->val);
			return reg;
		
		default : assert(0);
			
	}
}





void avm_callsaveenviroment(void)
{
	avm_push_envvalue(totalActuals);
	avm_push_envvalue(pc+1);
	avm_push_envvalue(top + totalActuals +2);
	avm_push_envvalue(topsp);
}




void execute_funcenter(instruction *instr)
{
	puts("execute_funcenter()");
	avm_memcell *func = avm_translate_operand(instr->result, &ax);
	assert(func);
	//assert(pc == func->data.funcVal);
	
	totalActuals = 0;
	userfunc1 *funcInfo = avm_getfuncinfo(pc);
	topsp = top;
	top = top - funcInfo->localsize;
}



void execute_funcexit(instruction *unused)
{
	puts("execute_funcexit()");
	unsigned oldTop = top;
	top = avm_get_envvalue(topsp + AVM_SAVEDTOP_OFFSET);
	pc = avm_get_envvalue(topsp + AVM_SAVEDPC_OFFSET);
	topsp = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFEST);
	
	while(oldTop++ <= top)
	{
		avm_memcellclear(  &avmstack[ oldTop ]  );
	}
}



// # WARNING
void execute_pusharg(instruction *instr)
{
	puts("execute_pusharg()");
	avm_memcell * arg = avm_translate_operand(instr->arg1, &ax);
	assert(arg);
	avm_assign( &avmstack[top], arg);
	totalActuals++;
	avm_dec_top();
}





void avm_calllibfunc(char *id)
{
	puts("avm_calllibfunc()");
	library_func_t f = avm_getlibraryfunc(id);
	
	if(!f)
	{
		avm_error("Unsupported lib func \"%s\" called!", id);
		executionFinished = 1;
	}
	else
	{
		/*
			Notice that enter function and exit function
			are called manually!
		*/
		topsp = top; // Enter funtion sequence. No avmstack locals.
		totalActuals = 0;
		(*f)();  // Call library function.
		
		if(!executionFinished)
			execute_funcexit( (instruction *)0 ); // return sequence.
		
	}
}




unsigned avm_totalactuals(void)
{
	puts("avm_totalactuals()");
	return avm_get_envvalue(topsp + AVM_NUMACTUALS_OFFSET);
}




avm_memcell *avm_getactual(unsigned i)
{
	puts("avm_getactual()");
	assert(i<avm_totalactuals());
	
	return &avmstack[ topsp + AVM_STACKENV_SIZE + 1 + i];
}







double add_impl(double x, double y)
{
	return x+y;
}


double sub_impl(double x, double y)
{
	return x-y;
}


double mul_impl(double x, double y)
{
	return x*y;
}

double div_impl(double x, double y)
{
	if (y!=0)
		return x/y; // Error check?
	else
		avm_error("Cannot divide by 0.");
			
}

double mod_impl(double x, double y)
{
	if(y!=0)
		return (  (unsigned)x   %   (unsigned)y  ); // Error check?
	else
		avm_error("Cannot divide by 0.");
}





void execute_arithmetic (instruction *instr)
{
	puts("execute_arithmetic()");
	avm_memcell *lv = avm_translate_operand(instr->result , (avm_memcell *)0 );
	avm_memcell *rv1 = avm_translate_operand(instr->arg1, &ax );
	avm_memcell *rv2 = avm_translate_operand(instr->arg2, &bx );

	assert(lv);
	assert(&avmstack[0]);
	assert(&avmstack[0] <=  lv);
	assert(&avmstack[top]);
	assert(&avmstack[0] <=  lv && &avmstack[top]> lv);
	
	assert(lv && (&avmstack[0] <=  lv && &avmstack[top]> lv || lv == &retval)  );
	assert(rv1 && rv2);
	
	if(rv1->type != number_m || rv2->type != number_m)
	{
		avm_error("not a number in arithmetic!");
		executionFinished = 1;
	}
	else
	{
		arithmetic_func_t op = arithmeticFuncs[ instr->opcode - add_v ];
		avm_memcellclear(lv);
		lv->type = number_m;
		lv->data.numVal = (*op)(rv1->data.numVal , rv2->data.numVal );
		puts("========== ");
	}
}






unsigned char number_tobool(avm_memcell *m)
{
	return m->data.numVal != 0;
}


unsigned char string_tobool(avm_memcell *m)
{
	return m->data.strVal[0] != 0;
}


unsigned char bool_tobool(avm_memcell *m)
{
	return m->data.boolVal;
}


unsigned char table_tobool(avm_memcell *m)
{
	return 1;
}


unsigned char userfunc_tobool(avm_memcell *m)
{
	return 1;
}


unsigned char libfunc_tobool(avm_memcell *m)
{
	return 1;
}


unsigned char nil_tobool(avm_memcell *m)
{
	return 0;
}

unsigned char undef_tobool(avm_memcell *m)
{
	assert(0);
	return 0;
}



unsigned char avm_tobool(avm_memcell *m)
{
	assert(m->type >= 0 && m->type < undef_m);
	
	return (*toboolFuncs[m->type])(m);
}


void execute_jeq(instruction *instr)
{
	assert(instr->result->type == label_a);
	
	avm_memcell *rv1 = avm_translate_operand( instr->arg1, &ax );
	avm_memcell *rv2 = avm_translate_operand( instr->arg2, &bx );
	
	unsigned char result = 0;
	
	if( rv1->type == undef_m || rv2->type == undef_m)
		avm_error(" \"undef\" involved in equality! ");
	
	else if ( rv1->type ==  nil_m || rv2->type == nil_m )
		result = rv1->type == nil_m && rv2->type == nil_m;
	
	else if( rv1->type == bool_m || rv2->type == bool_m )
		result = (avm_tobool(rv1) == avm_tobool(rv2));
	
	else if(rv1->type != rv2->type)
	{
		avm_error( "%s == %s is illegal!", typeStrings[rv1->type], typeStrings[rv2->type] );
	}
	
	else
	{
		if(rv1->type == number_m)
			result = (rv1->data.numVal == rv2->data.numVal);
		
		else if(rv1->type == string_m)
			result = (strcmp(rv1->data.strVal, rv2->data.strVal) == 0) ? 1:0 ;

		else if(rv1->type == table_m)
			assert(0);/*  WARNING */ 

		else if(rv1->type == userfunc_m)
			result = rv1->data.funcVal == rv2->data.funcVal;

		else if(rv1->type == libfunc_m)
			result = (strcmp(rv1->data.libfuncVal, rv2->data.libfuncVal) == 0) ? 1:0 ;
	}
		
	if( !executionFinished && result )
		pc = instr->result->val;
		
}


void execute_jne(instruction *instr){
	assert(instr->result->type == label_a);
	
	avm_memcell *rv1 = avm_translate_operand( instr->arg1, &ax );
	avm_memcell *rv2 = avm_translate_operand( instr->arg2, &bx );
	
	unsigned char result = 0;
	
	if( rv1->type == undef_m || rv2->type == undef_m)
		avm_error(" \"undef\" involved in equality! ");
	
	else if ( rv1->type ==  nil_m || rv2->type == nil_m )
		result = rv1->type == nil_m && rv2->type == nil_m;
	
	else if( rv1->type == bool_m || rv2->type == bool_m )
		result = (avm_tobool(rv1) == avm_tobool(rv2));
	
	else if(rv1->type != rv2->type)
	{
		avm_error( "%s == %s is illegal!", typeStrings[rv1->type], typeStrings[rv2->type] );
	}
	
	else
	{
		if(rv1->type == number_m)
			result = (rv1->data.numVal != rv2->data.numVal);
		
		else if(rv1->type == string_m)
			result = (strcmp(rv1->data.strVal, rv2->data.strVal) != 0) ? 1:0 ;
		
		else if(rv1->type == table_m)
			assert(0);/*  WARNING */
			
		else if(rv1->type == userfunc_m)
			result = rv1->data.funcVal != rv2->data.funcVal;
		
		else if(rv1->type == libfunc_m)
			result = (strcmp(rv1->data.libfuncVal, rv2->data.libfuncVal) != 0) ? 1:0 ;
	}
	if( !executionFinished && result )
			pc = instr->result->val;
		
}
void execute_jle(instruction *instr)/*8elei ulopoiisi*/{
	assert(instr->result->type == label_a);
	
	avm_memcell *rv1 = avm_translate_operand( instr->arg1, &ax );
	avm_memcell *rv2 = avm_translate_operand( instr->arg2, &bx );
	
	unsigned char result = 0;
	
	if( rv1->type == undef_m || rv2->type == undef_m)
		avm_error(" \"undef\" involved in equality! ");
	
	else if ( rv1->type ==  nil_m || rv2->type == nil_m )
		result = rv1->type == nil_m && rv2->type == nil_m;
	
	else if( rv1->type == bool_m || rv2->type == bool_m )
		result = (avm_tobool(rv1) == avm_tobool(rv2));
	
	else if(rv1->type != rv2->type)
	{
		avm_error( "%s == %s is illegal!", typeStrings[rv1->type], typeStrings[rv2->type] );
	}
	
	else
	{
		if(rv1->type == number_m)
			result = (rv1->data.numVal <= rv2->data.numVal);
		
		else if(rv1->type == string_m)
			result = (strcmp(rv1->data.strVal, rv2->data.strVal) <= 0) ? 1:0 ;
		
		else if(rv1->type == table_m)
			assert(0);/*  WARNING */ 
			
		else if(rv1->type == userfunc_m)
			result = rv1->data.funcVal <= rv2->data.funcVal;
		
		else if(rv1->type == libfunc_m)
			result = (strcmp(rv1->data.libfuncVal, rv2->data.libfuncVal) <= 0) ? 1:0 ;
	}
		
		if( !executionFinished && result )
			pc = instr->result->val;
		
}
void execute_jge(instruction *instr)/*8elei ulopoiisi*/{
	assert(instr->result->type == label_a);
	
	avm_memcell *rv1 = avm_translate_operand( instr->arg1, &ax );
	avm_memcell *rv2 = avm_translate_operand( instr->arg2, &bx );
	
	unsigned char result = 0;
	
	if( rv1->type == undef_m || rv2->type == undef_m)
		avm_error(" \"undef\" involved in equality! ");
	
	else if ( rv1->type ==  nil_m || rv2->type == nil_m )
		result = rv1->type == nil_m && rv2->type == nil_m;
	
	else if( rv1->type == bool_m || rv2->type == bool_m )
		result = (avm_tobool(rv1) == avm_tobool(rv2));
	
	else if(rv1->type != rv2->type)
	{
		avm_error( "%s == %s is illegal!", typeStrings[rv1->type], typeStrings[rv2->type] );
	}
	
	else
	{
		if(rv1->type == number_m)
			result = (rv1->data.numVal >= rv2->data.numVal);
		
		else if(rv1->type == string_m)
			result = (strcmp(rv1->data.strVal, rv2->data.strVal) >= 0) ? 1:0 ;
		
		else if(rv1->type == table_m)
			assert(0);/*  WARNING */ 
			
		else if(rv1->type == userfunc_m)
			result = rv1->data.funcVal >= rv2->data.funcVal;
		
		else if(rv1->type == libfunc_m)
			result = (strcmp(rv1->data.libfuncVal, rv2->data.libfuncVal) >= 0) ? 1:0 ;
	}
		
		if( !executionFinished && result )
			pc = instr->result->val;
		
}
void execute_jlt(instruction *instr)/*8elei ulopoiisi*/{
	assert(instr->result->type == label_a);
	
	avm_memcell *rv1 = avm_translate_operand( instr->arg1, &ax );
	avm_memcell *rv2 = avm_translate_operand( instr->arg2, &bx );
	
	unsigned char result = 0;
	
	if( rv1->type == undef_m || rv2->type == undef_m)
		avm_error(" \"undef\" involved in equality! ");
	
	else if ( rv1->type ==  nil_m || rv2->type == nil_m )
		result = rv1->type == nil_m && rv2->type == nil_m;
	
	else if( rv1->type == bool_m || rv2->type == bool_m )
		result = (avm_tobool(rv1) == avm_tobool(rv2));
	
	else if(rv1->type != rv2->type)
	{
		avm_error( "%s == %s is illegal!", typeStrings[rv1->type], typeStrings[rv2->type] );
	}
	
	else
	{
		if(rv1->type == number_m)
			result = (rv1->data.numVal < rv2->data.numVal);
		
		else if(rv1->type == string_m)
			result = (strcmp(rv1->data.strVal, rv2->data.strVal) < 0) ? 1:0 ;
		
		else if(rv1->type == table_m)
			assert(0);/*  WARNING */ 
			
		else if(rv1->type == userfunc_m)
			result = rv1->data.funcVal < rv2->data.funcVal;
		
		else if(rv1->type == libfunc_m)
			result = (strcmp(rv1->data.libfuncVal, rv2->data.libfuncVal) < 0) ? 1:0 ;
	}
		
		if( !executionFinished && result )
			pc = instr->result->val;
		
}
void execute_jgt(instruction *instr)/*8elei ulopoiisi*/{
	assert(instr->result->type == label_a);
	
	avm_memcell *rv1 = avm_translate_operand( instr->arg1, &ax );
	avm_memcell *rv2 = avm_translate_operand( instr->arg2, &bx );
	
	unsigned char result = 0;
	
	if( rv1->type == undef_m || rv2->type == undef_m)
		avm_error(" \"undef\" involved in equality! ");
	
	else if ( rv1->type ==  nil_m || rv2->type == nil_m )
		result = rv1->type == nil_m && rv2->type == nil_m;
	
	else if( rv1->type == bool_m || rv2->type == bool_m )
		result = (avm_tobool(rv1) == avm_tobool(rv2));
	
	else if(rv1->type != rv2->type)
	{
		avm_error( "%s == %s is illegal!", typeStrings[rv1->type], typeStrings[rv2->type] );
	}
	
	else
	{
		if(rv1->type == number_m)
			result = (rv1->data.numVal > rv2->data.numVal);
		
		else if(rv1->type == string_m)
			result = (strcmp(rv1->data.strVal, rv2->data.strVal) > 0) ? 1:0 ;
		
		else if(rv1->type == table_m)
			assert(0);/*  WARNING */ 
			
		else if(rv1->type == userfunc_m)
			result = rv1->data.funcVal > rv2->data.funcVal;
		
		else if(rv1->type == libfunc_m)
			result = (strcmp(rv1->data.libfuncVal, rv2->data.libfuncVal) > 0) ? 1:0 ;
	}
		
		if( !executionFinished && result )
			pc = instr->result->val;
		
}



void execute_newtable(instruction *instr)
{
	avm_memcell *lv = avm_translate_operand(instr->result, (avm_memcell *)0 );
	
	assert(lv && (&avmstack[0]<=lv && &avmstack[top]>lv || lv==&retval));
	
	avm_memcellclear(lv);
	
	lv->type = table_m;
	lv->data.tableVal = avm_tablenew();
	avm_tableincrefcounter(lv->data.tableVal);
}







void execute_tablegetelem(instruction *instr)
{
	avm_memcell *lv = avm_translate_operand( instr->result , (avm_memcell *)0 );
	avm_memcell *t = avm_translate_operand( instr->arg1, (avm_memcell *)0);
	avm_memcell *i = avm_translate_operand( instr->arg2, &ax );
	
	assert(lv && (&avmstack[0] <=  lv && &avmstack[top]> lv || lv == &retval)  );
	assert(t && &avmstack[0] <= t && &avmstack[top] > t  );
	assert(i);
	
	avm_memcellclear(lv);
	
	lv->type = nil_m;
	
	if(t->type!=table_m)
	{
		avm_error("illegal use of type %s as table!", typeStrings[t->type]);
	}
	else
	{
		avm_memcell *content = avm_tablegetelem(t->data.tableVal, i);
		
		if(content)
			avm_assign(lv, content);
		else
		{
			char *ts = avm_tostring(t);
			char *is = avm_tostring(i);
			
			avm_warning("%s[%s] not found!", ts, is);
			free(ts);
			free(is);
		}
	}
	
}




void execute_tablesetelem( instruction *instr)
{
	avm_memcell *t = avm_translate_operand( instr->result , (avm_memcell *)0 );
	avm_memcell *i = avm_translate_operand( instr->arg1, &ax);
	avm_memcell *c = avm_translate_operand( instr->arg2, &bx );
	
	assert(t && &avmstack[0] <= t && &avmstack[top] > t);
	assert(i && c);
	
	if(t->type != table_m)
		avm_error("illegal use of type %s as table!", typeStrings[t->type]);
	else
		avm_tablesetelem(t->data.tableVal, i, c);
}



avm_memcell *avm_tablegetelem( avm_table *table, avm_memcell *index )
{
	
}



void memclear_string(avm_memcell *m)
{
	assert(m->data.strVal);
	free(m->data.strVal);
}


void memclear_table(avm_memcell *m)
{
	assert(m->data.tableVal);
	avm_tabledecrefcounter(m->data.tableVal);
}



void avm_memcellclear(avm_memcell *m)
{
	if(m->type != undef_m )
	{
		memclear_func_t f = memclearFuncs[m->type];
		if(f)
		{
			(*f)(m);
		}
		m->type = undef_m;
	}
}


void execute_assign(instruction *instr)
{
	puts("execute_assign()");
	avm_memcell *lv = avm_translate_operand(instr->result, (avm_memcell *)0 );
	avm_memcell *rv = avm_translate_operand(instr->arg1, &ax );
	
	
	assert(lv && (&avmstack[0] <= lv && &avmstack[top] > lv || lv == &retval) );
	assert(rv);

	avm_assign(lv, rv);
		
}


void execute_call(instruction *instr)
{
	puts("execute_call()");
	avm_memcell *func = avm_translate_operand(instr->result, &ax);
	assert(func);
	avm_callsaveenviroment();

	switch(func->type)
	{
		case userfunc_m :

			pc = userfuncs[instr->result->val].address; //func->data.funcVal;
			assert(pc < AVM_ENDING_PC);
			assert(instructions[pc].opcode == funcenter_v);
			break;

		case string_m :
			avm_calllibfunc(func->data.strVal);
			break;

		case libfunc_m :
			avm_calllibfunc(func->data.libfuncVal);
			break;

		default :
		{
			char *s = avm_tostring(func);
			avm_error("calll: cannot bind \"%s\" to function!", s);
			free(s);
			executionFinished = 1;
		}
	}
}


void execute_nop(instruction *instr)
{
	
}




void avm_assign(avm_memcell *lv, avm_memcell *rv)
{
	enum memcell_t t = rv->type;
	puts("avm_assign()");

	
	if(lv == rv)
		return;

	if(lv->type == table_m && rv->type == table_m && lv->data.tableVal == rv->data.tableVal)
		return;

	
	if(rv->type == undef_m)
	{
		
		avm_warning("assigning from \"undef\" content!");
	}

	avm_memcellclear(lv);

	memcpy(lv, rv, sizeof(avm_memcell));
	
	if(lv->type == string_m)
		lv->data.strVal = strdup(rv->data.strVal);
	else if(lv->type == table_m)
		avm_tableincrefcounter(lv->data.tableVal);
}




void avm_dec_top(void)
{
	if(!top)
	{
		avm_error("stack overflow");
		executionFinished = 1;
	}
	else
		top--;
}




void avm_push_envvalue(unsigned val)
{
	avmstack[top].type = number_m;
	avmstack[top].data.numVal = val;
	avm_dec_top();
}



unsigned avm_get_envvalue(unsigned i)
{
	assert(avmstack[i].type == number_m);
	unsigned val = (unsigned) avmstack[i].data.numVal;
	assert(avmstack[i].data.numVal == ((double) val));
	return val;	
}

char * avm_tostring(avm_memcell * m){
	assert(m->type >=0 && m->type <= undef_m);
	return (*tostringfuncs[m->type])(m);
}

void avm_initialize(void){
	puts("avm_initialize()");
	avm_initstack();
	avm_registerlibfunc("print",libfunc_print);
	avm_registerlibfunc("typeof",libfunc_typeof);
	avm_registerlibfunc("input",libfunc_input);
	avm_registerlibfunc("objectmemberkeys",libfunc_objectmemberkeys);
	avm_registerlibfunc("objecttotalmembers",libfunc_objecttotalmembers);
	avm_registerlibfunc("objectcopy",libfunc_objectcopy);
	avm_registerlibfunc("totalarguments",libfunc_totalarguments);
	avm_registerlibfunc("argument",libfunc_argument);
	avm_registerlibfunc("srtonum",libfunc_srtonum);
	avm_registerlibfunc("sqrt",libfunc_sqrt);
	avm_registerlibfunc("cos",libfunc_cos);
	avm_registerlibfunc("sin",libfunc_sin);

}

double consts_getnumber(unsigned index){
	return numConsts[index];
}
char *consts_getstring(unsigned index){
	return stringConsts[index];
}
char *libfuncs_getused(unsigned index){
	return namedLibFuncs[index];
}




char* number_tostring (avm_memcell * m){
	assert(m->type == number_m);
	return itoa(m->data.numVal);
}

char* string_tostring (avm_memcell *m){
		assert(m->type == string_m);
		return m->data.strVal;
}
char* bool_tostring (avm_memcell *m){
	assert(m->type == bool_m);
	if(m->data.boolVal == 1)
		return "true";
	else
		return "false";
}
char* table_tostring (avm_memcell *m){/*8elei ulopoiisi*/
	
}
char* userfunc_tostring (avm_memcell *m){
	assert(m->type == userfunc_m);
	return userfuncs[m->data.funcVal].id;
}
char* libfunc_tostring (avm_memcell *m){
	assert(m->type == libfunc_m);
	return m->data.libfuncVal;
}
char* nil_tostring (avm_memcell *m){
	assert(m->type == nil_m);
	return "nil";
}
char* undef_tostring (avm_memcell *m){
	assert(m->type == undef_m);
	return "undefined";
}



char * itoa(int n){
	//const char *str = "";
	char *name ;
	char *s = malloc(1000*sizeof(char));
	int i, sign, c, j;
	
	if((sign = n) <0)
	{
		n=-n;
	}
	i=0;
	do{
		s[i++] = n%10 + '0';
	}while((n/=10)>0);
	if(sign <0)
	{
		s[i++] = '-';
	}
	s[i] = '\0';
	for(i=0, (j=strlen(s)-1); (i<j); i++, j--)
	{
		c=s[i];
		s[i] = s[j];
		s[j] = c;
	}
	//name=strdup(str);
	//strcat(name, s);
	char *str = malloc(sizeof(char) * strlen(s));
	strcpy(str, s);
	free(s);
	return str;
}



userfunc1 *avm_getfuncinfo(unsigned address)
{
	int i;
	for(i=0;i<totalUserFuncs;i++)
	{
		if( userfuncs[i].address == address )
			return &userfuncs[i];
	}
	return NULL;
}





avm_table *avm_tablenew(void)
{
	avm_table *t;
	t = malloc(sizeof(avm_table));
	AVM_WIPEOUT(*t);

	t->refcounter = t->total = 0;

	avm_tablebucketsinit(t->numIndexed);
	avm_tablebucketsinit(t->strIndexed);

	return t;
}


void avm_tabledestroy(avm_table *t)
{
	avm_tablebucketsdestroy(t->strIndexed);
	avm_tablebucketsdestroy(t->numIndexed);

	free(t);
}


void avm_tablebucketsdestroy(avm_tableBUCKET **p)
{
	unsigned i;
	avm_tableBUCKET *b;
	
	for(i=0;i<AVM_TABLE_HASHSIZE;i++)
	{
		for(b=*p; b;)
		{
			avm_tableBUCKET *del = b;
			b = b->next;
			avm_memcellclear(&del->key);
			avm_memcellclear(&del->value);

			free(del);
		}
		p[i] = (avm_tableBUCKET *)0;
	}	
}


void avm_tableincrefcounter(avm_table *t)
{
	++t->refcounter;
}



void avm_tabledecrefcounter(avm_table *t)
{
	assert(t->refcounter > 0);
	if(!--t->refcounter)
		avm_tabledestroy(t);
}



void avm_tablebucketsinit(avm_tableBUCKET **p)
{
	unsigned i;

	for(i=0;i<AVM_TABLE_HASHSIZE; i++)
	{
		p[i] = (avm_tableBUCKET *)0;
	}
}





static void avm_initstack(void)
{
	puts("avm_initstack()");
	unsigned i;
	for(i=0; i<AVM_STACKSIZE; i++)
	{
		AVM_WIPEOUT(avmstack[i]);
		avmstack[i].type = undef_m;
	}
}








void avm_tablesetelem( avm_table *table , avm_memcell *index, avm_memcell *content)
{
	
}



int isNumber(const char *s)
{
	int len = strlen(s);
	int i;
	int dot = 0;
	
	for(i=0;i<len;i++)
	{
		if(isdigit(s[i]))
			continue;
		else if(s[i] == '.')
		{
			dot++;
			if(dot==2)
				return 0;
		}
		else
			return 0;
	}
	return 1;
}