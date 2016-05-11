#include <stdio.h>
#include <stdlib.h>

#ifndef function_H
#define function_H


void WriteBack(){
	
	if(DM_WB.is_NOP){
		
	} else if(DM_WB.opcode == R && DM_WB.funct != jr){
		if(DM_WB.rd == 0){
			fprintf(error, "In cycle %d: Write $0 Error\n", cycle+1);
		} else {
			reg[DM_WB.rd] = DM_WB.ALUout;
		}
	} else if (DM_WB.opcode >= 32 && DM_WB.opcode <= 37){
		if(DM_WB.rt == 0){
			fprintf(error, "In cycle %d: Write $0 Error\n", cycle+1);
		} else {
			reg[DM_WB.rt] = DM_WB.MDR;
		}
	} else if (DM_WB.opcode >= 8  && DM_WB.opcode <= 15){
		if(DM_WB.rt == 0){
			fprintf(error, "In cycle %d: Write $0 Error\n", cycle+1);
		} else {
			reg[DM_WB.rt] = DM_WB.ALUout;
		}
	} else if (DM_WB.opcode == jal){
		reg[31] = DM_WB.PC + 4;
	}

	precedent.opcode = DM_WB.opcode;
	precedent.funct  = DM_WB.funct;
	precedent.MDR = DM_WB.MDR;
	precedent.is_NOP = DM_WB.is_NOP;
	precedent.rd = DM_WB.rd;
	precedent.rt = DM_WB.rt;
	precedent.ALUout = DM_WB.ALUout;
}

void PrintCycle(int PC, int cycle){ // after the cycle
	fprintf(snap, "PC: 0x%08X\n", IF_ID.PC);
	fprintf(snap, "IF: 0x%08X%s\n", IF_ID.instruction, (ID_EX.is_stall) ? " to_be_stalled" : (ID_EX.is_flush) ? " to_be_flushed" : "");
	
	fprintf(snap, "ID: %s%s", (ID_EX.is_NOP) ? "NOP" : (ID_EX.opcode == R) ? R_TYPE[ID_EX.funct] : I_TYPE[ID_EX.opcode], (ID_EX.is_stall) ? " to_be_stalled" : "");
	if(ID_EX.fwd.happen){
		if(ID_EX.fwd.rs)fprintf(snap, " fwd_EX-DM_rs_$%d", ID_EX.rs );
		if(ID_EX.fwd.rt)fprintf(snap, " fwd_EX-DM_rt_$%d", ID_EX.rt );	
	}
	fprintf(snap, "\n");
	
	fprintf(snap, "EX: %s", (EX_DM.is_NOP) ? "NOP" : (EX_DM.opcode == R) ? R_TYPE[EX_DM.funct] : I_TYPE[EX_DM.opcode]);
	if(EX_DM.fwd.happen){
		if(EX_DM.fwd.rs)fprintf(snap, " fwd_EX-DM_rs_$%d", EX_DM.fwd.rs );
		if(EX_DM.fwd.rt)fprintf(snap, " fwd_EX-DM_rt_$%d", EX_DM.fwd.rt );	
	}
	fprintf(snap, "\n");

	fprintf(snap, "DM: %s\n", (DM_WB.is_NOP) ? "NOP" : (DM_WB.opcode == R) ? R_TYPE[DM_WB.funct] : I_TYPE[DM_WB.opcode]);
	fprintf(snap, "WB: %s\n", (precedent.is_NOP) ? "NOP" : (precedent.opcode == R) ? R_TYPE[precedent.funct] : I_TYPE[precedent.opcode]);
	fprintf(snap, "\n\n");
}

void setInstructions(){ // fast transfer
	// R
	R_TYPE[32] = "ADD";
	R_TYPE[33] = "ADDU";
	R_TYPE[34] = "SUB";
	R_TYPE[36] = "AND";
	R_TYPE[37] = "OR";
	R_TYPE[38] = "XOR";
	R_TYPE[39] = "NOR";
	R_TYPE[40] = "NAND";
	R_TYPE[42] = "SLT";
	R_TYPE[0] = "SLL";
	R_TYPE[2] = "SRL";
	R_TYPE[3] = "SRA";
	R_TYPE[8] = "JR";
	// I


	I_TYPE[8] = "ADDI";
	I_TYPE[9] = "ADDIU";
	I_TYPE[35] = "LW";
	I_TYPE[33] = "LH";
	I_TYPE[37] = "LHU";
	I_TYPE[32] = "LB";
	I_TYPE[36] = "LBU";
	I_TYPE[43] = "SW";
	I_TYPE[41] = "SH";
	I_TYPE[40] = "SB";
	I_TYPE[15] = "LUI";
	I_TYPE[12] = "ANDI";
	I_TYPE[13] = "ORI";
	I_TYPE[14] = "NORI";
	I_TYPE[10] = "SLTI";
	I_TYPE[4] = "BEQ";
	I_TYPE[5] = "BNE";
	I_TYPE[7] = "BGTZ";
	// J
	I_TYPE[2] = "J";
	I_TYPE[3] = "JAL";
	// S
	I_TYPE[halt] = "HALT";
}
void InstrFetch(){ // seems to be ok
	if(PC < i_memory[0] || (PC-i_memory[0])/4 >= i_memory[1]) IF_ID.instruction = 0; // bound
	else IF_ID.instruction = i_memory[(PC-i_memory[0])/4 + 2]; // catch instruction
	IF_ID.PC = PC;
	IF_ID.is_flush = ID_EX.is_flush;
	PC = (ID_EX.is_stall) ? PC : (ID_EX.is_flush) ? is_branch : PC + 4; // decide next PC
}

void InstrDecode(){
	if(ID_EX.is_stall){ // only need to update reg
		ID_EX.reg_rs = reg[ID_EX.rs];
		ID_EX.reg_rt = reg[ID_EX.rt];
	} else if (IF_ID.is_flush){ // flush
		memset(&ID_EX, 0, sizeof(ID_EX));
		ID_EX.is_NOP = 1;
	} else { // normal
		ID_EX.PC = IF_ID.PC;
		ID_EX.opcode = IF_ID.instruction >> 26;
		if(ID_EX.opcode == R){	
			ID_EX.funct = IF_ID.instruction << 26 >> 26;
			ID_EX.rs    = IF_ID.instruction <<  6 >> 27;
			ID_EX.rt    = IF_ID.instruction << 11 >> 27;	
			ID_EX.reg_rs = reg[ID_EX.rs];
			ID_EX.reg_rt = reg[ID_EX.rt];
			ID_EX.rd	   = IF_ID.instruction << 16 >> 27;
			ID_EX.C	   = IF_ID.instruction << 21 >> 27;
		} else if (ID_EX.opcode >= 4 && ID_EX.opcode <= 43){
			short tempC;
			ID_EX.rs	   = IF_ID.instruction <<  6 >> 27;
			ID_EX.rt	   = IF_ID.instruction << 11 >> 27;
			ID_EX.reg_rs = reg[ID_EX.rs];
			ID_EX.reg_rt = reg[ID_EX.rt];
			tempC	   = IF_ID.instruction << 16 >> 16;
			if(ID_EX.opcode == andi || ID_EX.opcode == ori || ID_EX.opcode == nori){
				ID_EX.C = (unsigned short)tempC;
			} else { 
				ID_EX.C = tempC;
			}
		} else if (ID_EX.opcode == j || ID_EX.opcode == jal){
			ID_EX.C	   = IF_ID.instruction <<  6 >>  6;
		} else {
			// halt
		}
		ID_EX.is_NOP = ((IF_ID.instruction & 0xFC1FFFFF) == 0 ); // check NOP
	}
	
	// stall and forwarding     
	// may changed reg : rd -> R, rt -> I, $31 -> jal
	// may forwarding stage : EXDM(now in DMWB) -> ID, EXDM(now in EXDM) -> EX(now in here)
	// may forwarding instruction: R type, I type except load, jal
	ID_EX.fwd.happen = 0;
	EX_DM.predict = 0;
	int rtInEX_DM = (((EX_DM.opcode == R && EX_DM.funct != 8 && ID_EX.rt == EX_DM.rd) || (EX_DM.opcode >= 8 && EX_DM.opcode <= 37 && EX_DM.rt == ID_EX.rt) || (EX_DM.opcode == jal && ID_EX.rt == 31)) && (ID_EX.rt != 0));
	int rtInDM_WB = (((DM_WB.opcode == R && DM_WB.funct != 8 && ID_EX.rt == DM_WB.rd) || (DM_WB.opcode >= 8 && DM_WB.opcode <= 37 && DM_WB.rt == ID_EX.rt) || (DM_WB.opcode == jal && ID_EX.rt == 31)) && (ID_EX.rt != 0));
	int rsInEX_DM = (((EX_DM.opcode == R && EX_DM.funct != 8 && ID_EX.rs == EX_DM.rd) || (EX_DM.opcode >= 8 && EX_DM.opcode <= 37 && EX_DM.rt == ID_EX.rs) || (EX_DM.opcode == jal && ID_EX.rs == 31)) && (ID_EX.rs != 0));
	int rsInDM_WB = (((DM_WB.opcode == R && DM_WB.funct != 8 && ID_EX.rs == DM_WB.rd) || (DM_WB.opcode >= 8 && DM_WB.opcode <= 37 && DM_WB.rt == ID_EX.rs) || (DM_WB.opcode == jal && ID_EX.rs == 31)) && (ID_EX.rs != 0));
	int EX_DMforwarding = ((EX_DM.opcode == R && EX_DM.funct != 8) || (EX_DM.opcode >= 8 && EX_DM.opcode <= 15)  || (EX_DM.opcode == jal));
	int DM_WBforwarding = ((DM_WB.opcode == R && DM_WB.funct != 8) || (DM_WB.opcode >= 8 && DM_WB.opcode <= 15) || (DM_WB.opcode == jal));

	if(ID_EX.opcode == R && (ID_EX.funct == sll || ID_EX.funct == srl || ID_EX.funct == sra)){ // use only rt
		if(rtInEX_DM){
			if(EX_DMforwarding){
				EX_DM.predict = 2; // predict rt forwarding
				ID_EX.is_stall = 0; 
			} else { 
				ID_EX.is_stall = 1;
			}
		} else if (rtInDM_WB){
			ID_EX.is_stall = 1;
		} else {
			ID_EX.is_stall = 0;
		}
	} else if((ID_EX.opcode >= 7 && ID_EX.opcode <= 37 && ID_EX.opcode != lui) || (ID_EX.opcode == R && ID_EX.funct == jr)){ // use only rs
		if(rsInEX_DM){
			if(EX_DMforwarding && ID_EX.opcode != bgtz && ID_EX.opcode != R){
				EX_DM.predict = 1; // predict rs forwarding
				ID_EX.is_stall = 0; 
			} else { 
				ID_EX.is_stall = 1;
			}
		} else if (rsInDM_WB){
			if(DM_WBforwarding && (ID_EX.opcode == bgtz || (ID_EX.opcode == R && ID_EX.funct == jr))){
				ID_EX.fwd.happen = 1;
				ID_EX.fwd.rs = 1;
				ID_EX.fwd.rt = 0;
				ID_EX.reg_rs = DM_WB.ALUout;
				ID_EX.is_stall = 0;
			} else {
				ID_EX.is_stall = 1;
			}
		} else{
			ID_EX.is_stall = 0;
		}
	} else if (ID_EX.opcode != lui && ID_EX.opcode != j && ID_EX.opcode != jal && ID_EX.opcode != halt){ // use both rs and rt
		if(ID_EX.rs == ID_EX.rt){
			if(rsInEX_DM){
				if(EX_DMforwarding && (ID_EX.opcode != bne) && (ID_EX.opcode != beq)){
					EX_DM.predict = 3; // predict rs and rt forwarding
					ID_EX.is_stall = 0;
				} else {
					ID_EX.is_stall = 1;
				}
			} else if (rsInDM_WB){
				if(DM_WBforwarding && (ID_EX.opcode == bne || ID_EX.opcode == beq)){ // bne beq forwarding
					ID_EX.fwd.happen = 1;
					ID_EX.fwd.rs = 1;
					ID_EX.fwd.rt = 1;
					ID_EX.reg_rs = DM_WB.ALUout;
					ID_EX.reg_rt = DM_WB.ALUout;
					ID_EX.is_stall = 0;
				} else {
					ID_EX.is_stall = 1;
				}
			} else {
				ID_EX.is_stall = 0;
			}
		} else {
			if((rsInEX_DM && rtInDM_WB) || (rtInEX_DM && rsInDM_WB)){ // only stall
				ID_EX.is_stall = 1;
			} else if (rsInEX_DM || rtInEX_DM){ // both not in DMWB
				if(EX_DMforwarding && (ID_EX.opcode != bne) && (ID_EX.opcode != beq)){
					if(rsInEX_DM){ // rs forwarding
						EX_DM.predict = 1;
					} else { // rt forwarding
						EX_DM.predict = 2;
					}
					ID_EX.is_stall = 0;
				} else {
					ID_EX.is_stall = 1;
				}
			} else if (rsInDM_WB || rtInDM_WB){ // both not in EXDM
				if(DM_WBforwarding && (ID_EX.opcode == bne || ID_EX.opcode == beq)){ // bne beq forwarding
					ID_EX.fwd.happen = 1;
					ID_EX.fwd.rs = rsInDM_WB;
					ID_EX.fwd.rt = rtInDM_WB;
					if(rsInDM_WB) {
						ID_EX.reg_rs = DM_WB.ALUout;
					} else {
						ID_EX.reg_rt = DM_WB.ALUout;
					}
					ID_EX.is_stall = 0;
				} else {
					ID_EX.is_stall = 1;
				}
			} else {
				ID_EX.is_stall = 0; //no stall
			}
		}
	} else {
		ID_EX.is_stall = 0;
	}
	
	//branch check if no stall

	if(ID_EX.is_stall == 0){
		if(ID_EX.opcode == beq){
			is_branch = ID_EX.PC + 4 + ((ID_EX.reg_rs == ID_EX.reg_rt) ? ID_EX.C << 2 : 0);
			ID_EX.is_flush = (ID_EX.reg_rs == ID_EX.reg_rt) ? 1 : 0;
		} else if (ID_EX.opcode == bne){
			is_branch = ID_EX.PC + 4 + ((ID_EX.reg_rs != ID_EX.reg_rt) ? ID_EX.C << 2 : 0);
			ID_EX.is_flush = (ID_EX.reg_rs != ID_EX.reg_rt) ? 1 : 0;
		} else if (ID_EX.opcode == bgtz){
			is_branch = ID_EX.PC + 4 + (((int)ID_EX.reg_rs > 0) ? ID_EX.C << 2 : 0);
			ID_EX.is_flush = ((int)ID_EX.reg_rs > 0) ? 1 : 0;
		} else if(ID_EX.opcode == R && ID_EX.funct == jr){
			is_branch = ID_EX.reg_rs;
			ID_EX.is_flush = 1;
		} else if(ID_EX.opcode == j){
			is_branch = (ID_EX.PC + 4) >> 28 << 28 | (unsigned int)ID_EX.C << 2;
			ID_EX.is_flush = 1;
		} else if(ID_EX.opcode == jal){
			is_branch = (ID_EX.PC + 4) >> 28 << 28 | (unsigned int)ID_EX.C << 2;
			ID_EX.is_flush = 1;
		} else {
			ID_EX.is_flush = 0;
		}
	}
}

void Execute(){	
	// check stall and forwarding
	if(ID_EX.is_stall == 1){
		memset(&EX_DM, 0, sizeof(EX_DM));
		EX_DM.is_NOP = 1;
		return;
	} else if (EX_DM.predict > 0){
		EX_DM.fwd.happen = 1;
		EX_DM.fwd.rs = (EX_DM.predict == 1 || EX_DM.predict == 3) ? ID_EX.rs : 0;
		EX_DM.fwd.rt = (EX_DM.predict == 2 || EX_DM.predict == 3) ? ID_EX.rt : 0;
		if(EX_DM.fwd.rs) ID_EX.reg_rs = DM_WB.ALUout;
		if(EX_DM.fwd.rt) ID_EX.reg_rt = DM_WB.ALUout;
		EX_DM.predict = 0;
	} else {
		EX_DM.fwd.happen = 0;
	}
	
	// ALU
	if(ID_EX.opcode == R){	
		switch(ID_EX.funct){
			int s_sign, t_sign;
			case 32:
				s_sign = ID_EX.reg_rs >> 31;
				t_sign = ID_EX.reg_rt >> 31;
				EX_DM.ALUout = ID_EX.reg_rs + ID_EX.reg_rt;
				if(s_sign == t_sign && s_sign != EX_DM.ALUout >> 31)
					fprintf(error, "In cycle %d: Number Overflow\n", cycle+1);
				break;
			case 33:
				EX_DM.ALUout = ID_EX.reg_rs + ID_EX.reg_rt;
				break;
			case 34:
				s_sign = ID_EX.reg_rs >> 31;
				t_sign = (~ID_EX.reg_rt + 1) >> 31;
				EX_DM.ALUout = ID_EX.reg_rs + (~ID_EX.reg_rt + 1);
				if(s_sign == t_sign && s_sign != EX_DM.ALUout >> 31)
					fprintf(error, "In cycle %d: Number Overflow\n", cycle+1);
				break;
			case 36:
				EX_DM.ALUout = ID_EX.reg_rs & ID_EX.reg_rt;
				break;
			case 37:
				EX_DM.ALUout = ID_EX.reg_rs | ID_EX.reg_rt;
				break;
			case 38:
				EX_DM.ALUout = ID_EX.reg_rs ^ ID_EX.reg_rt;
				break;
			case 39:
				EX_DM.ALUout = ~(ID_EX.reg_rs | ID_EX.reg_rt);
				break;
			case 40:
				EX_DM.ALUout = ~(ID_EX.reg_rs & ID_EX.reg_rt);
				break;
			case 42:
				EX_DM.ALUout = ((int)ID_EX.reg_rs < (int)ID_EX.reg_rt);
				break;
			case 0:
				EX_DM.ALUout = ID_EX.reg_rt << ID_EX.C;
				break;
			case 2:
				EX_DM.ALUout = ID_EX.reg_rt >> ID_EX.C;
				break;
			case 3:
				EX_DM.ALUout = (int)ID_EX.reg_rt >> ID_EX.C;
				break;
			case 8:
				// no calculate
				break;
		}		
	} else if (ID_EX.opcode >= 4 && ID_EX.opcode <= 43){ // I instruction
		// IDEX.C as unsigned, signedC as signed
		int signedC = (int)ID_EX.C << 16 >> 16;
		int addr = (int)ID_EX.reg_rs + (int)signedC;
		// number overflow
		if(ID_EX.opcode >= 32 || ID_EX.opcode == 8){
			if((ID_EX.reg_rs >> 31) == ((unsigned)signedC >> 31) && (ID_EX.reg_rs >> 31) != ((unsigned)addr >> 31)){
				fprintf(error, "In cycle %d: Number Overflow\n", cycle+1);
			}
		}

		// don't know where should we do save word's rt mask
		if(ID_EX.opcode == addi || ID_EX.opcode == addiu || (ID_EX.opcode <= 43 && ID_EX.opcode >= 32)){
			EX_DM.ALUout = addr;
		} else if (ID_EX.opcode == lui){
			EX_DM.ALUout = ID_EX.C << 16;
		} else if (ID_EX.opcode == andi){
			EX_DM.ALUout = ID_EX.reg_rs & ID_EX.C;
		} else if (ID_EX.opcode == ori){
			EX_DM.ALUout = ID_EX.reg_rs | ID_EX.C;
		} else if (ID_EX.opcode == nori){
			EX_DM.ALUout = ~(ID_EX.reg_rs | ID_EX.C);
		} else if (ID_EX.opcode == slti){
			EX_DM.ALUout = ((int)ID_EX.reg_rs < signedC);
		}
	} else if (ID_EX.opcode == jal){
		EX_DM.ALUout = ID_EX.PC + 4;
	} else {
	 // halt
	}
	
	EX_DM.opcode = ID_EX.opcode;
	EX_DM.rd = ID_EX.rd;
	EX_DM.rt = ID_EX.rt;
	EX_DM.reg_rt = ID_EX.reg_rt;
	EX_DM.funct = ID_EX.funct;
	EX_DM.PC = ID_EX.PC;
	EX_DM.is_NOP = ID_EX.is_NOP;
}

void DataMemoryAccess(){

	int addr = EX_DM.ALUout;
	
	halt_error = 0;
	// address overflow
	if(EX_DM.opcode >= 32){
		if( ((EX_DM.opcode == lw || EX_DM.opcode == sw) && (addr > 1020 || addr < 0)) || 
			((EX_DM.opcode == lh || EX_DM.opcode == lhu || EX_DM.opcode == sh) && (addr > 1022 || addr < 0)) || 
		  	((EX_DM.opcode == lb || EX_DM.opcode == lbu || EX_DM.opcode == sb) && (addr > 1023 || addr < 0))){
			fprintf(error, "In cycle %d: Address Overflow\n", cycle+1);
			halt_error = 1;
		}
	}
	// misalignment error
	if(EX_DM.opcode >= 32){
		if( ((EX_DM.opcode == lw || EX_DM.opcode == sw) && (addr % 4) != 0) || 
			((EX_DM.opcode == lh || EX_DM.opcode == lhu || EX_DM.opcode == sh) && (addr % 2) != 0)){
			fprintf(error, "In cycle %d: Misalignment Error\n", cycle+1);
			halt_error = 1;	
		}
	}
	
	if(halt_error == 1){
		// prepare to halt
	} else if (EX_DM.opcode == lw){
		DM_WB.MDR = d_memory[addr] << 24 | d_memory[addr+1] << 16 | d_memory[addr+2] << 8 | d_memory[addr+3];	
	} else if (EX_DM.opcode == lh){
		DM_WB.MDR = (char)d_memory[addr] << 8 | (unsigned char)d_memory[addr+1];
	} else if (EX_DM.opcode == lhu){
		DM_WB.MDR = (unsigned char)d_memory[addr] << 8 | (unsigned char)d_memory[addr+1];
	} else if (EX_DM.opcode == lb){
		DM_WB.MDR = (char)d_memory[addr];
	} else if (EX_DM.opcode == lbu){
		DM_WB.MDR = (unsigned char)d_memory[addr];
	 } else if (EX_DM.opcode == sw){
		d_memory[addr]   = EX_DM.reg_rt >> 24;
		d_memory[addr+1] = EX_DM.reg_rt << 8  >> 24;
		d_memory[addr+2] = EX_DM.reg_rt << 16 >> 24;
		d_memory[addr+3] = EX_DM.reg_rt << 24 >> 24;
	} else if (EX_DM.opcode == sh){
		d_memory[addr]   = EX_DM.reg_rt << 16 >> 24;
		d_memory[addr+1] = EX_DM.reg_rt << 24 >> 24;
	} else if (EX_DM.opcode == sb){
		d_memory[addr]   = EX_DM.reg_rt << 24 >> 24;
	}
	
	
	DM_WB.ALUout = EX_DM.ALUout;
	DM_WB.opcode = EX_DM.opcode;
	DM_WB.funct = EX_DM.funct;
	DM_WB.rd = EX_DM.rd;
	DM_WB.rt = EX_DM.rt;
	DM_WB.PC = EX_DM.PC;
	DM_WB.is_NOP = EX_DM.is_NOP;
}
#endif
