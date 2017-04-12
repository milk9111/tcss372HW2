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



int sext(int immed7) {
	if (HIGH_ORDER_BIT_VALUE & immed7) return (immed7 | 0xFFC0);
	else return immed7;
}

void trap(int trap_vector) {
	//if (trap_vector == 0x0020) { //GETC
	//} else if (trap_vector == 0x0021) { //OUT
	//} else if (trap_vector == 0x0022) { //PUTS
	//} else if (trap_vector == 0x0023) { //IN
	//} else if (trap_vector == 0x0024) { //PUTSP
	if (trap_vector == 0x0025) { //HALT
		
	}
	
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
                printf("Here in FETCH\n");
				cpu->mar = cpu->pc;
				cpu->pc++;							//State 18
				cpu->mdr = memory[cpu->mar];		//State 33
				cpu->ir = cpu->mdr;					//State 35
				
				printf("Contents of PC = 0x%4X\n", cpu->pc);
				printf("Contents of MAR = 0x%4X\n", cpu->mar);
				printf("Contents of PC = 0x%4X\n", cpu->pc);
				printf("Contents of M[MAR] = 0x%4X\n", memory[cpu->mar]);
				printf("Contents of MDR = 0x%4X\n", cpu->mdr);
                printf("Contents of IR = %04X\n", cpu->ir);
				//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                // put printf statements in each state and microstate to see that it is working
 				//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
               state = DECODE;
                break;
            case DECODE: // microstate 32
				opcode = (cpu->ir & OPCODE_FIELD) >> OPCODE_FIELD_SHIFT;
				switch (opcode) {
					case ADD:
					case AND:
						Rd = (cpu->ir & RD_FIELD) >> RD_FIELD_SHIFT;
						Rs1 = (cpu->ir & RS1_FIELD) >> RS1_FIELD_SHIFT;
						if (!(HIGH_ORDER_BIT_VALUE & cpu->ir)){	
							Rs2 = (cpu->ir & RS2_FIELD) >> RS2_FIELD_SHIFT;	
						} else {
							immed5 = (cpu->ir & IMMED5_FIELD) >> IMMED5_FIELD_SHIFT;
						}
						break;
					case NOT:
						Rd = (cpu->ir & RD_FIELD) >> RD_FIELD_SHIFT;
						Rs1 = (cpu->ir & RS1_FIELD) >> RS1_FIELD_SHIFT;
						break;
					case TRAP:
						trapVector8 = (cpu->ir & TRAP_VECTOR8_FIELD) >> TRAP_VECTOR8_FIELD_SHIFT;
						break;
					case LD:
						Rd = (cpu->ir & RD_FIELD) >> RD_FIELD_SHIFT;
						offset9 = (cpu->ir & OFFSET9_FIELD) >> OFFSET9_FIELD_SHIFT;
						break;
					case ST:
						Rs1 = (cpu->ir & RD_FIELD) >> RD_FIELD_SHIFT;
						offset9 = (cpu->ir & OFFSET9_FIELD) >> OFFSET9_FIELD_SHIFT;
						break;
					case JMP:
						BaseR = (cpu->ir & RS1_FIELD) >> RS1_FIELD_SHIFT;
						break;
					case BR:
						offset9 = (cpu->ir & OFFSET9_FIELD) >> OFFSET9_FIELD_SHIFT;
				}
				
                // get the fields out of the IR
                // make sure opcode is in integer form
				// hint: use four unsigned int variables, opcode, Rd, Rs, and immed7
				// extract the bit fields from the IR into these variables
                state = EVAL_ADDR;
                break;
            case EVAL_ADDR: // Look at the LD instruction to see microstate 2 example
                switch (opcode) {
					case ADI:
					case ANI:
						immed5 = sext (IMMED5_FIELD & cpu->ir);
						break;
					case TRAP:
						break;
						
					/*
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
					*/
                // different opcodes require different handling
                // compute effective address, e.g. add sext(immed7) to register
                }
                state = FETCH_OP;
                break;
            case FETCH_OP: // Look at ST. Microstate 23 example of getting a value out of a register
				switch (opcode) {
                    // get operands out of registers into A, B of ALU
                    // or get memory for load instr.
					case ST:
						// state 23 MDR <- SR
						cpu->mdr = Rd;
						break;
					case LD:
						// state 25 MDR <- M[MAR]
						cpu->mdr = memory[cpu->mar];
						
					case TRAP:
						//state 28 PC<-MDR
						cpu->pc = cpu->mdr;
						break;
                }
                state = EXECUTE;
                break;
            case EXECUTE: // Note that ST does not have an execute microstate
                switch (opcode) {
                    // do what the opcode is for, e.g. ADD
                    // in case of TRAP: call trap(int trap_vector) routine, see below for TRAP x25 (HALT)
					case ADD:
						//DR<-SR1 + OP2
						//set CC
						Rd = Rs1 + Rs2;
						if (Rd > 0) {
							cpu->n = 0;
							cpu->z = 0;
							cpu->p = 1;
						} else if (Rd == 0) {
							cpu->n = 0;
							cpu->z = 1;
							cpu->p = 0;
						} else {
							cpu->n = 1;
							cpu->z = 0;
							cpu->p = 0;
						}
						break;
					case AND:
						Rd = Rs1 & Rs2;
						if (Rd > 0) {
							cpu->n = 0;
							cpu->z = 0;
							cpu->p = 1;
						} else if (Rd == 0) {
							cpu->n = 0;
							cpu->z = 1;
							cpu->p = 0;
						} else {
							cpu->n = 1;
							cpu->z = 0;
							cpu->p = 0;
						}
						break;
					case NOT:
						Rd = !Rs1;
						if (Rd > 0) {
							cpu->n = 0;
							cpu->z = 0;
							cpu->p = 1;
						} else if (Rd == 0) {
							cpu->n = 0;
							cpu->z = 1;
							cpu->p = 0;
						} else {
							cpu->n = 1;
							cpu->z = 0;
							cpu->p = 0;
						}
						break;
					case TRAP:
						trap(trapVector8);
						break;
					case BR:
						
						break;
					case JMP:
						cpu->pc = BaseR;
						break;
                }
                state = STORE;
                break;
            case STORE: // Look at ST. Microstate 16 is the store to memory
                switch (opcode) {
					case ST:
						memory[cpu->mar] = cpu->mdr;
						break;
					case LD:
						Rd = cpu->mdr;
						if (Rd > 0) {
							cpu->n = 0;
							cpu->z = 0;
							cpu->p = 1;
						} else if (Rd == 0) {
							cpu->n = 0;
							cpu->z = 1;
							cpu->p = 0;
						} else {
							cpu->n = 1;
							cpu->z = 0;
							cpu->p = 0;
						}
						break;
					case TRAP:
						//state 30?
						break;
                    // write back to register or store MDR into memory
                }
                // do any clean up here in prep for the next complete cycle
                state = FETCH;
                break;
        }
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
	controller (cpu);
	return 0;
}






