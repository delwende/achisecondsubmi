#ifndef declaration_H
#define declaration_H
#define $sp 29
// R
#define R 0
#define add 32   
#define addu 33  
#define sub 34   
#define and 36   
#define or 37    
#define xor 38   
#define nor 39   
#define nand 40  
#define slt 42   
#define sll 0    
#define srl 2    
#define sra 3    
#define jr 8     
// I
#define addi 8   
#define addiu 9  
#define lw 35    
#define lh 33    
#define lhu 37   
#define lb 32    
#define lbu 36   
#define sw 43    
#define sh 41    
#define sb 40    
#define lui 15   
#define andi 12  
#define ori 13   
#define nori 14  
#define slti 10  
#define beq 4    
#define bne 5    
#define bgtz 7   
// J
#define j 2      
#define jal 3    
// S
#define halt 63  

FILE   *error, *snap;

int fileSize_d,fileSize_i;


unsigned int reg[32];
int cycle;
int halt_error;
int is_branch;

typedef struct forwarding{ //forwarding
	int happen;
	int rs;
	int rt;
}forward;
typedef struct IDtoEX{
	unsigned PC;
	unsigned opcode;
	unsigned rs;
	unsigned reg_rs;
	unsigned rt;
	unsigned reg_rt;
	unsigned rd;
	unsigned C; //shamt, immediate, address
	unsigned funct;
	unsigned is_NOP;
	unsigned is_stall;
	forward fwd;
	unsigned is_flush;
}fromID_to_EX;
typedef struct EXtoDM{
	unsigned PC;
	unsigned opcode;
	unsigned ALUout;
	unsigned rd;
	unsigned rt;
	unsigned reg_rt;
	unsigned funct;
	unsigned is_NOP;
	unsigned predict; // 0 as not, 1 as rs, 2 as rt, 3 as both
	forward fwd;
}formEX_to_DM;

typedef struct IFtoID{
	unsigned instruction;
	unsigned PC;
	unsigned is_stall;
	unsigned is_flush;
}formIF_to_ID;



typedef struct DMtoWB{
	unsigned PC;
	unsigned opcode;
	unsigned MDR;
	unsigned ALUout;
	unsigned rd;
	unsigned rt;
	unsigned funct;
	unsigned is_NOP;
}formDM_to_WB;


formDM_to_WB DM_WB;
formIF_to_ID IF_ID;

formDM_to_WB precedent;
fromID_to_EX ID_EX;
formEX_to_DM EX_DM;

char* R_TYPE[45];
char* I_TYPE[65];

unsigned int PC, i_memory[1026];
unsigned int sp, d_data[1026];
unsigned char d_memory[1024];
void execute_pipeline();
void register_output();
void WriteBack();
		void DataMemoryAccess();
		void Execute();
		void InstrDecode();
		void InstrFetch();
		void PrintCycle(int PC, int cycle);
		void initialize();
#endif