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


void trap(int trap_vector) {
	//if (trap_vector == 0x0020) { //GETC
	//} else if (trap_vector == 0x0021) { //OUT
	//} else if (trap_vector == 0x0022) { //PUTS
	//} else if (trap_vector == 0x0023) { //IN
	//} else if (trap_vector == 0x0024) { //PUTSP
	if (trap_vector == 25) { //HALT
		exit(0);
	}
}


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
    int state = FETCH, BEN;
    for (;;) {   // efficient endless loop
        switch (state) {
            case FETCH: // microstates 18, 33, 35 in the book
                printf("Here in FETCH\r\n\r\n");
				printf("Contents of PC = 0x%04X\r\n", cpu->pc);
				cpu->mar = cpu->pc;
				cpu->pc++;							//State 18
				cpu->mdr = memory[cpu->mar];		//State 33
				cpu->ir = cpu->mdr;					//State 35
				
				
				printf("Contents of MAR = 0x%04X\r\n", cpu->mar);
				printf("Contents of PC = 0x%04X\r\n", cpu->pc);
				printf("Contents of M[MAR] = 0x%04X\r\n", memory[cpu->mar]);
				printf("Contents of MDR = 0x%04X\r\n", cpu->mdr);
                printf("Contents of IR = 0x%04X\r\n", cpu->ir);
				//exit(0);
				//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                // put printf statements in each state and microstate to see that it is working
 				//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
               state = DECODE;
                break;
            case DECODE: // microstate 32
			printf("\r\nHere in DECODE\r\n\r\n");
				opcode = (cpu->ir & OPCODE_FIELD) >> OPCODE_FIELD_SHIFT;
				switch (opcode) {
					case ADD:
					case AND:
						Rd = (cpu->ir & RD_FIELD) >> RD_FIELD_SHIFT;
						Rs1 = (cpu->ir & RS1_FIELD) >> RS1_FIELD_SHIFT;
						printf("Contents of Rd = %d\r\n", Rd);
						printf("Contents of Rs1 = %d\r\n", Rs1);
						if (!(HIGH_ORDER_BIT_VALUE & cpu->ir)){	
							Rs2 = (cpu->ir & RS2_FIELD) >> RS2_FIELD_SHIFT;	
							printf("Contents of Rs2 = %d\r\n", Rs2);
						} else {
							immed5 = (cpu->ir & IMMED5_FIELD) >> IMMED5_FIELD_SHIFT;
							immed5 = sext5(IMMED5_FIELD & cpu->ir);
							printf("Contents of immed5 = %d\r\n", immed5);
						}
						break;
					case NOT:
						Rd = (cpu->ir & RD_FIELD) >> RD_FIELD_SHIFT;
						Rs1 = (cpu->ir & RS1_FIELD) >> RS1_FIELD_SHIFT;
						printf("Contents of Rd = %d\r\n", Rd);
						printf("Contents of Rs = %d\r\n", Rs1);
						break;
					case TRAP:
						trapVector8 = (cpu->ir & TRAP_VECTOR8_FIELD) >> TRAP_VECTOR8_FIELD_SHIFT;
						printf("Contents of Trap Vector 8 = %d\r\n", trapVector8);
						break;
					case LD:
						Rd = (cpu->ir & RD_FIELD) >> RD_FIELD_SHIFT;
						offset9 = (cpu->ir & OFFSET9_FIELD) >> OFFSET9_FIELD_SHIFT;
						printf("Contents of Rd = %d\r\n", Rd);
						printf("Contents of PC Offset 9 = %d\r\n", offset9);
						break;
					case ST:
						Rs1 = (cpu->ir & RD_FIELD) >> RD_FIELD_SHIFT;
						offset9 = (cpu->ir & OFFSET9_FIELD) >> OFFSET9_FIELD_SHIFT;
						printf("Contents of Rs = %d\r\n", Rs1);
						printf("Contents of PC Offset 9 = %d\r\n", offset9);
						break;
					case JMP:
						BaseR = (cpu->ir & RS1_FIELD) >> RS1_FIELD_SHIFT;
						printf("Contents of BaseR = %d\r\n", BaseR);
						break;
					case BR:
						offset9 = (cpu->ir & OFFSET9_FIELD) >> OFFSET9_FIELD_SHIFT;
						printf("Contents of PC Offset 9 = %d\r\n", offset9);
						break;
				}
				BEN = ((cpu->ir & 0x0800) & cpu->n) | ((cpu->ir & 0x0400) & cpu->z) | ((cpu->ir & 0x0200) & cpu->p);  
                // get the fields out of the IR
                // make sure opcode is in integer form
				// hint: use four unsigned int variables, opcode, Rd, Rs, and immed7
				// extract the bit fields from the IR into these variables
                state = EVAL_ADDR;
                break;
            case EVAL_ADDR: // Look at the LD instruction to see microstate 2 example
				printf ("\r\nHere in EVAL_ADDR\r\n\r\n");
                switch (opcode) {
					case LD:
					case ST:
						cpu->mar = cpu->pc + sext9(OFFSET9_FIELD & cpu->ir);
						break;
					case TRAP:
						cpu->mar = TRAP_VECTOR8_FIELD & cpu->ir;
						break;
                }
				printf ("Contents of MAR = %d\r\n", cpu->mar);
                state = FETCH_OP;
                break;
            case FETCH_OP: // Look at ST. Microstate 23 example of getting a value out of a register
				printf ("\r\nHere in FETCH_OP\r\n\r\n");
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
						break;
					case TRAP:
						//state 28 PC<-MDR
						//cpu->mdr = memory[cpu->mar];
						//cpu->reg_file[7] = cpu->pc;
						/*
							This is what it would do in a full simulation.
						*/
						break;
                }
                state = EXECUTE;
                break;
            case EXECUTE: // Note that ST does not have an execute microstate
				printf ("\r\nHere in EXECUTE\r\n\r\n");
                switch (opcode) {
                    // do what the opcode is for, e.g. ADD
                    // in case of TRAP: call trap(int trap_vector) routine, see below for TRAP x25 (HALT)
					case ADD:
						printf("ADD\r\n");
						//DR<-SR1 + OP2
						//set CC
						cpu->reg_file[Rd] = cpu->reg_file[Rs1] + cpu->reg_file[Rs2];
						printf ("Executed DR = SR1 + SR2\r\n");
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
						printf("\r\nAND\r\n");
						cpu->reg_file[Rd] = cpu->reg_file[Rs1] & cpu->reg_file[Rs2];
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
						printf ("Executed DR = SR1 & SR2\r\n");
						break;
					case NOT:
						printf("NOT\r\n");
						cpu->reg_file[Rd] = !cpu->reg_file[Rs1];
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
						printf ("Executed DR = SR\r\n");
						break;
					case TRAP:
						printf("TRAP\r\n");
						printf("Executed Trap Vector #%d\r\n", trapVector8);
						trap(cpu->mar);
						break;
					case BR:
						if (BEN) {
							printf("\r\nBR\r\n");
							printf ("Executed PC = PC + OFFSET9\r\n");
							cpu->pc = cpu->pc + sext9(offset9);
						}
						break;
					case JMP:
						printf("\r\nJMP\r\n");
						printf ("Executed PC = BaseR\r\n");
						cpu->pc = BaseR;
						break;
                }
				
                state = STORE;
                break;
            case STORE: // Look at ST. Microstate 16 is the store to memory
				printf ("\r\nHere in STORE\r\n\r\n");
                switch (opcode) {
					case ST:
						memory[cpu->mar] = cpu->mdr;
						printf("Stored MDR into M[MAR]\r\n");
						break;
					case LD:
						Rd = cpu->mdr;
						printf("Stored MDR into DR\r\n");
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
						//cpu->pc = cpu->mdr;
						/*
							This is what it would do in a full simulation.
						*/
						break;
                    // write back to register or store MDR into memory
                }
                // do any clean up here in prep for the next complete cycle
                state = FETCH;
				printf("Value of R0: %d\r\n", cpu->reg_file[0]);
				printf("Value of R1: %d\r\n", cpu->reg_file[1]);
				printf("Value of R2: %d\r\n", cpu->reg_file[2]);
				printf("Value of R3: %d\r\n", cpu->reg_file[3]);
				printf ("\r\n\r\n");
                break;
        }
    }
	return 0;
}


int main (int argc, char* argv[]) {
	/*memory[0] = 0x1642;
	memory[1] = 0x1662;
	memory[2] = 0x5642;
	memory[3] = 0x566F;
	memory[4] = 0x967F;
	//memory[5] = 0xF019;
	memory[5] = 0x967E;
	memory[6] = 0x2004;
	memory[7] = 0x3605;
	//memory[7] = 0xC000;
	memory[8] = 0xF019;
	*/
	char *temp;
	memory[0] = strtol(argv[1], &temp, 16);
	memory[1] = 0xF019;
	CPU_p cpu = malloc (sizeof(CPU_s));
	cpu->n = 1;
	cpu->reg_file[0] = 12;
	cpu->reg_file[1] = 4;
	cpu->reg_file[2] = 19;
	cpu->reg_file[3] = 5;
	cpu->pc = 0;
	printf("Value of R0: %d\r\n", cpu->reg_file[0]);
	printf("Value of R1: %d\r\n", cpu->reg_file[1]);
	printf("Value of R2: %d\r\n", cpu->reg_file[2]);
	printf("Value of R3: %d\r\n", cpu->reg_file[3]);
	controller (cpu);
	return 0;
}






