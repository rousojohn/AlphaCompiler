#include "avm.h"


void fetchInstructions(FILE *fp)
{

	if(fp==NULL)
		return;
	
	int i;


	// NUMBERS
	
	fread(&totalNumConsts, sizeof(int), 1, fp);

	numConsts = malloc(sizeof(double) * totalNumConsts);

	fread(numConsts, sizeof(double), totalNumConsts, fp);



	
	// STINGS

	fread(&totalStringConsts, sizeof(int), 1, fp);
	stringConsts = malloc(sizeof(char *) * totalStringConsts);

	for(i=0;i<totalStringConsts;i++)
	{
		int len;
		fread(&len, sizeof(int), 1, fp);
		stringConsts[i] = malloc(sizeof(char)*len);
		fread(stringConsts[i], sizeof(char), len, fp);
	}



	// LIBRARY FUNCTIONS

	fread(&totalNamedLibFuncs, sizeof(int), 1, fp);
	namedLibFuncs = malloc(sizeof(char *) * totalNamedLibFuncs);

	for(i=0;i<totalNamedLibFuncs;i++)
	{
		int len;
		fread(&len, sizeof(int), 1, fp);
		namedLibFuncs[i] = malloc(sizeof(char)*len);
		fread(namedLibFuncs[i], sizeof(char), len, fp);
	}



	// USER FUNCTIONS

	fread(&totalUserFuncs, sizeof(int), 1, fp);
	userfuncs = malloc(sizeof(userfunc1) * totalUserFuncs);

	for(i=0;i<totalUserFuncs;i++)
	{
		fread(&userfuncs[i].address, sizeof(unsigned), 1, fp);
		fread(&userfuncs[i].localsize, sizeof(unsigned), 1, fp);
		
		int len;
		fread(&len, sizeof(int), 1, fp);
		userfuncs[i].id = malloc(sizeof(char)*len);
		fread(userfuncs[i].id, sizeof(char), len, fp);
	}



	// INSTRUCTIONS

	fread(&totalInstructions, sizeof(int), 1, fp);
	instructions = malloc(sizeof(instruction) * totalInstructions);

	

	for(i=0;i<totalInstructions;i++)
	{
		fread(&instructions[i].opcode, sizeof(enum vmopcode), 1, fp);
		
		int tmp;

		fread(&tmp, sizeof(int), 1, fp);

		// if tmp == NULL
		if(tmp==-2)
		{
			instructions[i].result = malloc(sizeof(vmarg));
			fread(&instructions[i].result->type, sizeof(enum vmarg_t), 1, fp);
			fread(&instructions[i].result->val, sizeof(unsigned), 1, fp);

			if(instructions[i].result->type == global_a)
			{
				unsigned index = instructions[i].result->val;
				if( index > totalGlobalVars)
					totalGlobalVars = index;
			}
		}



		fread(&tmp, sizeof(int), 1, fp);

		// if tmp == NULL
		if(tmp==-2)
		{
			instructions[i].arg1 = malloc(sizeof(vmarg));
			fread(&instructions[i].arg1->type, sizeof(enum vmarg_t), 1, fp);
			fread(&instructions[i].arg1->val, sizeof(unsigned), 1, fp);

			if(instructions[i].arg1->type == global_a)
			{
				unsigned index = instructions[i].arg1->val;
				if( index > totalGlobalVars)
					totalGlobalVars = index;
			}
		}




		fread(&tmp, sizeof(int), 1, fp);
		
		// if tmp == NULL
		if(tmp==-2)
		{
			instructions[i].arg2 = malloc(sizeof(vmarg));
			fread(&instructions[i].arg2->type, sizeof(enum vmarg_t), 1, fp);
			fread(&instructions[i].arg2->val, sizeof(unsigned), 1, fp);

			if(instructions[i].arg2->type == global_a)
			{
				unsigned index = instructions[i].arg2->val;
				if( index > totalGlobalVars)
					totalGlobalVars = index;
			}
		}

		fread(&instructions[i].srcLine, sizeof(unsigned), 1, fp);
	}
	fclose(fp);

	codeSize = totalInstructions;
	
}





void init_global_variables()
{
	globalVars = malloc(sizeof(avm_memcell) * totalGlobalVars+1);
	avm_memcell tmp;
	tmp.type = undef_m;
	int i;
	for(i=0;i<totalGlobalVars;i++)
		globalVars[i] = tmp;
		
}



int main(int argc,char * argv[])
{
	
	FILE *fp;
	if(argc >= 1 )
	{	
		if( (fp=fopen(argv[1],"rb")) == NULL)
		{
			fprintf(stderr,"\n Could not open the file \"%s\"\n\n", argv[1]);
			return 1;
		}
	}
	else
	{
		fprintf(stderr, "\n No input files \n\n");
		exit(1);
	}



	executionFinished = 0;
	pc = 0;
	currLine = 0;
	totalActuals = 0;
	topsp = 0;
	top = AVM_STACKSIZE-1;

	totalGlobalVars = 0;
	globalVars = (avm_memcell *)0;


	avm_initialize();

	
	fetchInstructions(fp);


	init_global_variables();

	
	while(!executionFinished)
	{
		execute_cycle();
	}

	return 0;
}
