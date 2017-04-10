/*
	Toy Computer Simulation V.1
	
	cpu.c
	
	Programmer: Connor Lundberg, Michael Wilson
	Date: 3/3/2017
	
*/

#include "cpu.h"
//#include "alu.h"
// create these files after figuring out all of the functions
//#include "cpu_complete2.h"


int initALU(ALU_P a) {
	a->A = 0;
	a->B = 0;
	a->R = 0;
}

int initCPU (CPU_P c, ALU_P a) {
	int i;
	for (i=0; i<NO_REGISTERS; i++) c->reg_file[i] = 0;
	c->alu = a;
	c->PC = 0; 
	c->MAR = 0; 
	c->MDR = 0; 
	c->IR = 0;
	for (int i = 0; i < FLAGS; i++) c->flags[i] = 0;
}


int sext(int immed7) {
	printf ("0x%X\n", HIGH_ORDER_BIT_VALUE & immed7);
	if (HIGH_ORDER_BIT_VALUE & immed7) {printf("in here2\n"); return (immed7 | 0xFFC0);}
	else {printf("in here3\n"); return immed7;}
}


int sext11(int immed11) {
	if (HIGH_ORDER_BIT_VALUE11 & immed11) return (immed11 | 0xFC00);
	else return immed11;
}


void setFlags (CPU_P cpu, unsigned int neg, unsigned int zero, unsigned int overflow) {
	cpu->flags[0] = neg;
	cpu->flags[1] = zero;
	cpu->flags[2] = overflow;
}


int overflowCheck (unsigned int result) {
	int check = 0;
	
	if (0x10000 & result) check = 1;
	
	return check;
}


int negativeCheck (unsigned int result) {
	int check = 0;
	
	if (0x8000 & result) check = 1;
	
	return check;
}


int branchChoice (CPU_P cpu, unsigned int defn, unsigned int effective_addr) {
	int wasChanged = 0;
	switch (defn) {
		case JUMP:
			setFlags (cpu, 1, 1, 1);
			cpu->reg_file[7] = cpu->PC + 1;
			cpu->PC = effective_addr;
			wasChanged = 1;
		case NEG:
			if (cpu->flags[0]) {
				cpu->PC = effective_addr;
				wasChanged = 1;
			}
			break;
		case ZERO:
			if (cpu->flags[1]) {
				cpu->PC = effective_addr;
				wasChanged = 1;
			}
			break;
		case OVERFLOW:
			if (cpu->flags[2]) {
				printf ("overflow happened\n");
				cpu->PC = effective_addr;
				wasChanged = 1;
			}
			break;
	}
	
	return wasChanged;
}


int controller (CPU_P cpu) {
	if (!cpu) return -1;
	int state, wasChanged = 0;
	unsigned int opcode, Rd, Rs, immed7, immed, effective_addr, defn;
	unsigned int immed11;
	//defn is used for the 2 bits in BR and TRAP to further define them.
    state = FETCH;
    for (;;) {
        switch (state) {
            case FETCH: 
				cpu->IR = memory[cpu->PC];
                state = DECODE;
                break;
            case DECODE:
				opcode = (OPCODE_FIELD & cpu->IR) >> OPCODE_FIELD_SHIFT;
				switch (opcode) {
					case ADD:
					case NAND:
					case ADI:
					case LDI:
					case LD:
					case ST:
						Rd = (RD_FIELD & cpu->IR) >> RD_FIELD_SHIFT;
						Rs = (RS_FIELD & cpu->IR) >> RS_FIELD_SHIFT;
						immed7 = (IMMED_FIELD & cpu->IR) >> IMMED_FIELD_SHIFT;
						break;
					case BR:
					case TRAP:
						defn = (BR_FIELD & cpu->IR) >> BR_FIELD_SHIFT;
						immed11 = (IMMED11_FIELD & cpu->IR) >> IMMED11_FIELD_SHIFT;
						break;
				}
				
                state = EVAL_ADDR;
                break;
            case EVAL_ADDR:
                switch (opcode) {
					case ADD:
					case NAND:
						Rd = (RD_FIELD & cpu->IR) >> RD_FIELD_SHIFT;
						Rs = (RS_FIELD & cpu->IR) >> RS_FIELD_SHIFT;
						break;
					case ADI:
						immed = sext(IMMED_FIELD & cpu->IR);
						Rs = (RS_FIELD & cpu->IR) >> RS_FIELD_SHIFT;
						break;
					case LDI:
						immed = sext(IMMED_FIELD & cpu->IR);
						cpu->MAR = immed;
						break;
					case LD:
					case ST:
						effective_addr = Rs + sext(IMMED_FIELD & cpu->IR);
						cpu->MAR = effective_addr;
						break;
					case BR:
						effective_addr = cpu->PC + sext11(IMMED11_FIELD & cpu->IR);
						break;
					case TRAP: break;
                }
                state = FETCH_OP;
                break;
            case FETCH_OP:
                switch (opcode) {
					case NAND:
					case ADD:
						cpu->alu->A = cpu->reg_file[Rd];
						cpu->alu->B = cpu->reg_file[Rs];
						break;
					case ADI:
						cpu->alu->A = cpu->reg_file[Rd];
						printf ("0x%X\n", immed);
						cpu->alu->B = immed;
						break;
					case LDI:
						cpu->MDR = immed;
						break;
					case LD:
						cpu->MDR = memory[cpu->MAR];
						break;
					case ST:
						memory[cpu->MAR] = cpu->MDR;
						break;
					case BR:
					case TRAP: break;
                }
                state = EXECUTE;
                break;
            case EXECUTE:
                switch (opcode) {
 					case ADD:
					case ADI:
						cpu->alu->R = cpu->alu->A + cpu->alu->B;
						
						if (overflowCheck (cpu->alu->R)) { 		//overflow (both operands have same sign but result is opposite)
							setFlags (cpu, 0, 0, 1);
						} else if (cpu->alu->R == 0) { 		//zero
							setFlags (cpu, 0, 1, 0);
						} else if (negativeCheck (cpu->alu->R)) {		//negative
							setFlags (cpu, 1, 0, 0);
						} else { 		//positive
							setFlags (cpu, 0, 0, 0);
						}
						break;
					case NAND:
						cpu->alu->R = ~(cpu->alu->A & cpu->alu->B);
						break;
					case LDI:
						break;
					case LD:
						cpu->MDR = memory[cpu->MAR];
						break;
					case ST:
						memory[cpu->MAR] = cpu->MDR;
						break;
					case BR:
						wasChanged = branchChoice (cpu, defn, effective_addr);
						break;
					case TRAP: break;
                }
                state = STORE;
                break;
            case STORE:
                switch (opcode) {
 					case ADD:
					case ADI:
					case NAND:
						cpu->reg_file[Rd] = cpu->alu->R;
						break;
					case LDI:
						cpu->reg_file[Rd] = immed;
						break;
					case LD:
						cpu->reg_file[Rd] = cpu->MDR;
						break;
					case ST:
						memory[cpu->MAR] = cpu->MDR;
						break;
					case BR: break;
					case TRAP: return 1;
                }
				if (!wasChanged) {
					cpu->PC++;
					wasChanged = 0;
				}
                state = FETCH;
				//for (int i = 0; i < FLAGS; i++) cpu->flags[i] = 0;
                break;
        }

    }
}


/*
	The main controller of the program, handles the 
	initilization of the CPU and ALU, starts the debugger,
	and uses the choice made by the user.
*/
void debugController () {
	int choice = 0, startPoint = 0, e = 0, programLoaded = 0;
	int notCleared = 1;
	
	char *fileName = (char *) malloc (MAX_NAME_LENGTH * sizeof(char));
	
	ALU_P alu_p = malloc(sizeof(ALU_S));
	initALU(alu_p);
	CPU_P cpu_p = malloc(sizeof(CPU_S));
	initCPU(cpu_p, alu_p);
	FILE *infile;
	
	
	clearMem (cpu_p);
	choice = printScreen (0, cpu_p);
	
	while (choice < 5 && choice > 0) {
		switch (choice) {
			case 1:
				if (notCleared) {
					printf ("Please enter the .hex file: ");
					scanf ("%s", fileName);
					if (checkExtension (fileName)) {
						notCleared = 0;
						programLoaded = 1;
						infile = fopen (fileName, "r");
						loadFile (&infile);
						clearScreen ();
						choice = printScreen (0, cpu_p);
						fclose(infile);
					} else {
						clearScreen ();
						printf ("Incorrect file name, please try again\n\n");
						choice = printScreen (0, cpu_p);
					}
				} else {
					clearScreen ();
					printf ("Memory has not been cleared yet\n\n");
					choice = printScreen (0, cpu_p);
				}
				break;
			case 2:
				e = controller (cpu_p);
				clearScreen ();
				choice = printScreen (0, cpu_p);
				break;
			case 3:
				notCleared = 1;
				clearMem (cpu_p);
				clearScreen ();
				choice = printScreen (0, cpu_p);
				break;
			case 4:
				printf ("Starting Address (in hex): ");
				scanf ("%x", &startPoint);
				clearScreen ();
				choice = printScreen (startPoint, cpu_p);
				break;
		}
	}
}


/*
	The main function used to get the whole program started!
*/
int main (int argc, char **argv) {
	debugController ();
	return 0;
}










