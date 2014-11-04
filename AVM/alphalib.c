#include "avm.h"





void libfunc_argument(void)
{
	
}






void libfunc_objectmemberkeys(void)
{
	
}






void libfunc_objecttotalmembers(void)
{
	
}





void libfunc_objectcopy(void)
{
	
}





void libfunc_print(void)
{
	unsigned n = avm_totalactuals();
	unsigned i;
	for(i=0; i<n; i++)
	{
		char *s = avm_tostring(avm_getactual(i));
		puts(s);
		free(s);
	}
}








void libfunc_sqrt(void)
{
	unsigned n = avm_totalactuals();
	if(n!=1)
		avm_error("one argument  (not %d)  expected in \"sqrt\"!", n);
	else
	{
		avm_memcellclear(&retval);
		retval.type = number_m;

		if(avm_getactual(0)->data.numVal >0)
			retval.data.numVal = sqrt( avm_getactual(0)->data.numVal );
		else
		{
			retval.type = nil_m;
		}
	}
}







void libfunc_cos(void){
	unsigned n = avm_totalactuals();
	if(n!=1)
		avm_error("one argument  (not %d)  expected in \"sqrt\"!", n);
	else
	{
		avm_memcellclear(&retval);
		retval.type = number_m;
		retval.data.numVal = cos( avm_getactual(0)->data.numVal );
	}
}








void libfunc_sin(void){
	unsigned n = avm_totalactuals();
	if(n!=1)
		avm_error("one argument  (not %d)  expected in \"sqrt\"!", n);
	else
	{
		avm_memcellclear(&retval);
		retval.type = number_m;
		retval.data.numVal = sin( avm_getactual(0)->data.numVal );
	}
}









void libfunc_srtonum(void){
	unsigned n = avm_totalactuals();
	if(n!=1)
		avm_error("one argument  (not %d)  expected in \"sqrt\"!", n);
	else
	{
		avm_memcellclear(&retval);
		retval.type = number_m;
		if( ! isNumber(avm_getactual(0)->data.strVal))
		{
			retval.type = nil_m;
		}
		else
			retval.data.numVal = atoi( avm_getactual(0)->data.strVal);
	}
}








void libfunc_typeof(void)
{
	unsigned n = avm_totalactuals();
	
	if(n!=1)
		avm_error("one argument  (not %d)  expected in \"typeof\"!", n);
	else
	{
		avm_memcellclear(&retval);
		retval.type = string_m;
		retval.data.strVal = strdup( typeStrings[ avm_getactual(0)->type ] );
	}
}








void libfunc_input(void)
{ // EKTELEITAI WS X=INPUT();
	unsigned n = avm_totalactuals();
	char * in;
	if(n!=1)
		avm_error("one argument  (not %d)  expected in \"typeof\"!", n);
	else
	{
		avm_memcellclear(&retval);
		scanf("%s", in);
		if(isNumber(in))
		{
			retval.type=number_m;
			retval.data.numVal=atol(in);
		}
		else{
			if(strcmp(in, "true")==0 || strcmp(in, "false")==0)
			{
				retval.type = bool_m;
			}
			else
				retval.type = string_m;
			
			retval.data.strVal=strdup(in);
		}
	}
}






void libfunc_totalarguments()
{
	unsigned p_topsp = avm_get_envvalue( topsp + AVM_SAVEDTOPSP_OFFEST);
	
	avm_memcellclear(&retval);
	
	if(!p_topsp)
	{
		avm_error("\"totalarguments\" called outside a function");
		retval.type = nil_m;
	}
	else
	{
		retval.type = number_m;
		retval.data.numVal = avm_get_envvalue(p_topsp + AVM_NUMACTUALS_OFFSET);
	}
}

