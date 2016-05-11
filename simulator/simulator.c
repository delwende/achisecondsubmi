#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "declaration.h"
#include "function.h"
 int main()
{error = fopen("error_dump.rpt", "w");
	snap = fopen("snapshot.rpt", "w");

	/* code */

	Read_DMEMORY();
	Read_IMEMORY();

	initialize();
	setInstructions();
	execute_pipeline();
	fclose(error);
	fclose(snap);
	return 0;
}

void initialize(){
	// struct set
	memset(&IF_ID, 0, sizeof(IF_ID));
	memset(&ID_EX, 0, sizeof(ID_EX));
	memset(&EX_DM, 0, sizeof(EX_DM));
	memset(&DM_WB, 0, sizeof(DM_WB));
	memset(&precedent, 0, sizeof(precedent));
	ID_EX.is_NOP = 1;
	EX_DM.is_NOP = 1;
	DM_WB.is_NOP = 1;
	precedent.is_NOP = 1;


}

void execute_pipeline(){

	
	cycle = 0;

	while(1){

			
		
		
		register_output();
		WriteBack();
		DataMemoryAccess();
		Execute();
		InstrDecode();
		InstrFetch();
		PrintCycle(PC, cycle++);
		if(halt_error == 1) return;
		if(ID_EX.opcode == halt && EX_DM.opcode == halt && DM_WB.opcode == halt && precedent.opcode == halt)
			return;
	}
}
void register_output(){ // before the cycle
	int i;
	fprintf(snap, "cycle %d\n", cycle);
	for(i = 0; i < 32; i++){
		fprintf(snap, "$%02d: 0x%08X\n", i, reg[i]);
	}
}

