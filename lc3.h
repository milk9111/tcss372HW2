/*
	Author: Connor Lundberg
	Date: 4/4/2017
*/

#define FETCH 0
#define DECODE 1
#define EVAL_ADDR 2
#define FETCH_OP 3
#define EXECUTE 4
#define STORE 5

#define NO_OF_REGISTERS 8

#define OPCODE_FIELD 0xE000				// 1110 0000 0000 0000 - gets first three bits
#define OPCODE_FIELD_SHIFT 13
#define RD_FIELD 0x1C00					// 0001 1100 0000 0000 - gets Rd field bits
#define RD_FIELD_SHIFT 10
#define RS_FIELD 0x03800				// 0000 0011 1000 0000 - ges Rs field bits
#define RS_FIELD_SHIFT 7
#define IMMED_FIELD 0x007F				// 0000 0000 0111 1111 - gets immed field bits
#define IMMED_FIELD_SHIFT 6
#define BR_FIELD 0x1800					// 0001 1000 0000 0000 - gets branch field bits
#define BR_FIELD_SHIFT 11
#define IMMED11_FIELD 0x07FF			// 0000 0111 1111 1111 - gets immed 11 field bits
#define IMMED11_FIELD_SHIFT 10
#define HIGH_ORDER_BIT_VALUE 0x0070		// 0000 0000 0100 0000
#define HIGH_ORDER_BIT_VALUE11 0X0700	// 0000 0100 0000 0000


typedef unsigned short Register;

typedef struct alu_s {
	Register a;
	Register b;
	Register r;
} ALU_s;

typedef ALU_s *ALU_p;

typedef struct cpu_s {
	Register ir;
	Register pc;
	Register mar;
	Register mdr;
	Register reg_file[NO_OF_REGISTERS];
} CPU_s;

typedef CPU_s *CPU_p;

