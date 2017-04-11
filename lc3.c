/*
	Author: Connor Lundberg, James Roberts
	Date: 4/4/2017
*/
#include <stdio.h>
#include <stdlib.h>
#include "lc3.h"

//current version


// you can define a simple memory module here for this program
unsigned short memory[32];   // 32 words of memory enough to store simple program



int sext5(int immed5) {
	if (HIGH_ORDER_BIT_VALUE & immed5) return (immed5 | 0xFFC0);
	else return immed5;
}


int sext9(int offset9) {
	if (HIGH_ORDER_BIT_VALUE9 & offset9) return (offset9 | 0xFD00);
	else return offset9;
}


int controller (CPU_p cpu) {
    // check to make sure both pointers are not NULL
    // do any initializations here
	Register opcode, Rd, Rs1, Rs2, immed5, offset9;	// fields for the IR
	Register effective_addr, trapVector8, BaseR;
    int state = FETCH, BEN, n, z, p;
    for (;;) {   // efficient endless loop
        switch (state) {
            case FETCH: // microstates 18, 33, 35 in the book
                printf("Here in FETCH\n\n");
				printf("Contents of PC = 0x%04X\n", cpu->pc);
				cpu->mar = cpu->pc;
				cpu->pc++;							//State 18
				cpu->mdr = memory[cpu->mar];		//State 33
				cpu->ir = cpu->mdr;					//State 35
				
				
				printf("Contents of MAR = 0x%04X\n", cpu->mar);
				printf("Contents of PC = 0x%04X\n", cpu->pc);
				printf("Contents of M[MAR] = 0x%04X\n", memory[cpu->mar]);
				printf("Contents of MDR = 0x%04X\n", cpu->mdr);
                printf("Contents of IR = 0x%04X\n", cpu->ir);
				//exit(0);
				//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                // put printf statements in each state and microstate to see that it is working
 				//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
               state = DECODE;
                break;
            case DECODE: // microstate 32
			printf("\nHere in DECODE\n\n");
				opcode = (cpu->ir & OPCODE_FIELD) >> OPCODE_FIELD_SHIFT;
				switch (opcode) {
					case ADD:
					case AND:
						Rd = (cpu->ir & RD_FIELD) >> RD_FIELD_SHIFT;
						Rs1 = (cpu->ir & RS1_FIELD) >> RS1_FIELD_SHIFT;
						printf("Contents of Rd = %d\n", Rd);
						printf("Contents of Rs1 = %d\n", Rs1);
						if (!(HIGH_ORDER_BIT_VALUE & cpu->ir)){	
							Rs2 = (cpu->ir & RS2_FIELD) >> RS2_FIELD_SHIFT;	
							printf("Contents of Rs2 = %d\n", Rs2);
						} else {
							immed5 = (cpu->ir & IMMED5_FIELD) >> IMMED5_FIELD_SHIFT;
							immed5 = sext5(IMMED5_FIELD & cpu->ir);
							printf("Contents of immed5 = %d\n", immed5);
						}
						break;
					case NOT:
						Rd = (cpu->ir & RD_FIELD) >> RD_FIELD_SHIFT;
						Rs1 = (cpu->ir & RS1_FIELD) >> RS1_FIELD_SHIFT;
						printf("Contents of Rd = %d\n", Rd);
						printf("Contents of Rs = %d\n", Rs1);
						break;
					case TRAP:
						trapVector8 = (cpu->ir & TRAP_VECTOR8_FIELD) >> TRAP_VECTOR8_FIELD_SHIFT;
						printf("Contents of Trap Vector 8 = %d\n", trapVector8);
						break;
					case LD:
						Rd = (cpu->ir & RD_FIELD) >> RD_FIELD_SHIFT;
						offset9 = (cpu->ir & OFFSET9_FIELD) >> OFFSET9_FIELD_SHIFT;
						printf("Contents of Rd = %d\n", Rd);
						printf("Contents of PC Offset 9 = %d\n", offset9);
						break;
					case ST:
						Rs1 = (cpu->ir & RD_FIELD) >> RD_FIELD_SHIFT;
						offset9 = (cpu->ir & OFFSET9_FIELD) >> OFFSET9_FIELD_SHIFT;
						printf("Contents of Rs = %d\n", Rs1);
						printf("Contents of PC Offset 9 = %d\n", offset9);
						break;
					case JMP:
						BaseR = (cpu->ir & RS1_FIELD) >> RS1_FIELD_SHIFT;
						printf("Contents of BaseR = %d\n", BaseR);
						break;
					case BR:
						offset9 = (cpu->ir & OFFSET9_FIELD) >> OFFSET9_FIELD_SHIFT;
						printf("Contents of PC Offset 9 = %d\n", offset9);
						break;
				}
				
                // get the fields out of the IR
                // make sure opcode is in integer form
				// hint: use four unsigned int variables, opcode, Rd, Rs, and immed7
				// extract the bit fields from the IR into these variables
                state = EVAL_ADDR;
                break;
            case EVAL_ADDR: // Look at the LD instruction to see microstate 2 example
				printf ("\nHere in EVAL_ADDR\n\n");
                switch (opcode) {
					case LD:
					case ST:
						cpu->mar = cpu->pc + sext9(OFFSET9_FIELD & cpu->ir);
						break;
					case TRAP:
						cpu->mar = TRAP_VECTOR8_FIELD & cpu->ir;
						break;
                }
				printf ("Contents of MAR = %d\n", cpu->mar);
                state = FETCH_OP;
                break;
            case FETCH_OP: // Look at ST. Microstate 23 example of getting a value out of a register
                switch (opcode) {
                    // get operands out of registers into A, B of ALU
                    // or get memory for load instr.
                }
                state = EXECUTE;
                break;
            case EXECUTE: // Note that ST does not have an execute microstate
				if (cpu->ir == END) {
					exit(0);
				}
                switch (opcode) {
                    // do what the opcode is for, e.g. ADD
                    // in case of TRAP: call trap(int trap_vector) routine, see below for TRAP x25 (HALT)
                }
                state = STORE;
                break;
            case STORE: // Look at ST. Microstate 16 is the store to memory
                switch (opcode) {
                    // write back to register or store MDR into memory
                }
                // do any clean up here in prep for the next complete cycle
                state = FETCH;
                break;
        }
		printf ("\n");
    }
	return 0;
}


int main (int argc, char* argv[]) {
	memory[0] = 0x1642;
	memory[1] = 0x1662;
	memory[2] = 0x5642;
	memory[3] = 0x566F;
	memory[4] = 0x967F;
	memory[5] = 0xF019;
	memory[6] = 0x2004;
	memory[7] = 0x3605;
	memory[8] = 0xC000;
	memory[9] = 0x0E14;
	CPU_p cpu = malloc (sizeof(CPU_s));
	cpu->pc = 0;
	controller (cpu);
	return 0;
}






