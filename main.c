#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define VALUE_MASK 4095

#define LDI 0
#define LDM 1
#define STR 2
#define ADD 3
#define SUB 4
#define JNZ 5
#define SWB 6
#define SWC 7
#define SWD 8
#define SWE 9
#define OR 10
#define AND 11
#define NOT 12
#define SHL 13
#define SHR 14
#define HLT 15

char * opcodes[16] = {
  "ldi", "ldm", "str", "add", "sub", "jnz", "swb", "swc", "swd", "swe", "or", "and", "not", "shl", "shr", "hlt"
};

struct Registers {
  uint8_t a, b, c, d, e;
  uint16_t ip : 12;
} regs;

uint8_t memory[8192] = {
  LDI << 4, 3,
  SWD << 4, 0,
  LDI << 4, 5,
  SWB << 4, 0,
  ADD << 4, 0,
  SWE << 4, 0,
  SWD << 4, 0,
  SWB << 4, 0,
  LDI << 4, 1,
  SWC << 4, 0,
  SUB << 4, 0,
  SWD << 4, 0,
  SWE << 4, 0,
  SWC << 4, 0,
  LDI << 4, 4,
  JNZ << 4, 0,
  HLT << 4, 0,
};

uint16_t instruction = 0;
uint8_t hlt = 0;

void ldi() {
  regs.a = instruction & 0xFF;
}

void ldm() {
  regs.a = memory[instruction & VALUE_MASK];
}

void str() {
  memory[instruction & VALUE_MASK] = regs.a;
}

void add() {
  regs.a = regs.b + regs.c;
}

void sub() {
  regs.a = regs.b - regs.c;
}

void jnz() {
  if (regs.d != 0) {
    regs.ip = regs.a + (instruction & VALUE_MASK);
  }
}

void swb() {
  uint8_t tmp = regs.a;
  regs.a = regs.b;
  regs.b = tmp;
}

void swc() {
  uint8_t tmp = regs.a;
  regs.a = regs.c;
  regs.c = tmp;
}

void swd() {
  uint8_t tmp = regs.a;
  regs.a = regs.d;
  regs.d = tmp;
}

void swe() {
  uint8_t tmp = regs.a;
  regs.a = regs.e;
  regs.e = tmp;
}

void ior() {
  regs.a = regs.b | regs.c;
}

void iand() {
  regs.a = regs.b & regs.c;
}

void inot() {
  regs.a = ~regs.b;
}

void shl() {
  regs.a = regs.b << regs.c;
}

void shr() {
  regs.a = regs.b >> regs.c;
}

void fetch_instruction() {
  instruction = (memory[regs.ip] << 8) | (memory[regs.ip + 1]);
  regs.ip += 2;
}

void run_instruction() {
  switch (instruction >> 12) {
    case LDI:
      ldi();
      break;
    case LDM:
      ldm();
      break;
    case STR:
      str();
      break;
    case ADD:
      add();
      break;
    case SUB:
      sub();
      break;
    case JNZ:
      jnz();
      break;
	case SWB:
		swb();
		break;
	case SWC:
		swc();
		break;
	case SWD:
		swd();
		break;
	case SWE:
		swe();
		break;
	case OR:
		ior();
		break;
	case AND:
		iand();
		break;
	case NOT:
		inot();
		break;
	case SHL:
		shl();
		break;
	case SHR:
		shr();
		break;
	case HLT:
		hlt = 1;
		break;
	default:
		break;
	}
}

void print_status() {
	printf("\na: 0x%02x\t\tb: 0x%02x\t\tc: 0x%02x\t\td: 0x%02x\t\te: 0x%02x\t\tip: 0x%03x\t\topcode: %s\tvalue: 0x%03x", regs.a, regs.b, regs.c, regs.d, regs.e, regs.ip, opcodes[instruction >> 12], instruction & VALUE_MASK);
}

void loop() {
	fetch_instruction();
	run_instruction();
	print_status();

	if (hlt) {
		printf("\n\nHALT!\n\n");

		while (1);
	}
}

int main(int argc, char ** argv) {
	FILE * binfp;

	if (argc >= 2) {
		if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
			printf("USAGE: %s {binary file}\nIf no file is provided it will run an embedded program\n", argv[0]);
			return -1;
		}

		binfp = fopen(argv[1], "r");
		if (binfp == NULL) {
			printf("File %s can not be read from\n", argv[1]);
			return 1;
		}

		fseek(binfp, 0L, SEEK_END);
		size_t size = ftell(binfp);
		fseek(binfp, 0L, SEEK_SET);

		if (fread(memory, 1, size, binfp) != size) {
			printf("Error whilst reading from file %s\n", argv[1]);
			return 2;
		}

		fclose(binfp);
	}

	printf("\n\n\n\n\n\n\n\n\n\n\n\nBOOT:\n\n\n");
	print_status();

	while (1) {
		loop();
		for (size_t i = 0; i < 0xFFFF; i++);
	}

	printf("Emulator exited\n");

	return 0;
}
