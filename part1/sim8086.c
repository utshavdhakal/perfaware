#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "util.h"

typedef unsigned char byte;

FILE* file;

char* DecodeRegister(byte reg, byte width) {
	switch(reg) {
		case 0b000: return width == 0 ? "al" : "ax";
		case 0b001: return width == 0 ? "cl" : "cx";
		case 0b010: return width == 0 ? "dl" : "dx";
		case 0b011: return width == 0 ? "bl" : "bx";
		case 0b100: return width == 0 ? "ah" : "sp";
		case 0b101: return width == 0 ? "ch" : "bp";
		case 0b110: return width == 0 ? "dh" : "si";
		case 0b111: return width == 0 ? "bh" : "di";
	}
}

char* DecodeMemory(byte REG_MEM, byte MOD) {
	char* MEM = (char*) malloc(20);
	char* MEM_WITH_DISP = (char*) malloc(20);

	switch (REG_MEM) {
		case 0b000: MEM = "bx + si"; break;
		case 0b001: MEM = "bx + di"; break;
		case 0b010: MEM = "bp + si"; break;
		case 0b011: MEM = "bp + di"; break;
		case 0b100: MEM = "si"; break;
		case 0b101: MEM = "di"; break;
		case 0b110:
			if (MOD == 0b00) MEM = "DIRECT ADDRESS?";
			else MEM = "bp"; break;
		case 0b111: MEM = "bx"; break;
	}

	if (MOD == 0b00) return MEM;

	else if (MOD == 0b01) {
		char DISP = ReadByte();
		
		if (DISP == 0) return MEM;
		else if (DISP > 0) sprintf(MEM_WITH_DISP, "%s + %d", MEM, DISP);

		return MEM_WITH_DISP;
	}

	else if (MOD == 0b10) {
		short DISP = ReadWord();

		if (DISP == 0) return MEM;
		else if (DISP > 0) sprintf(MEM_WITH_DISP, "%s + %hi", MEM, *((short*) &DISP));

		return MEM_WITH_DISP;
	}
}

char* DecodeRegisterMemory(byte MOD, byte REG_MEM, byte WIDTH) {
	switch (MOD) {
		case 0b00:
		case 0b01:
		case 0b10:
			return DecodeMemory(REG_MEM, MOD);
		case 0b11:
			return DecodeRegister(REG_MEM, WIDTH);
	}
}

void ParseCopyRegisterMemoryToFromRegister(byte first) {
	byte second;
	second = ReadByte();

	byte WIDTH = first & 0b00000001;
	byte MOD = (second >> 6) & 0b00000011;
	byte DIR = (first >> 1) & 0b00000001;

	char *REG = DecodeRegister((second >> 3) & 0b00000111, WIDTH);
	char *REG_MEM = DecodeRegisterMemory(MOD, second & 0b00000111, WIDTH);

	if (MOD == 0b11) {
		if (DIR) printf("mov %s, %s\n", REG, REG_MEM);
		else printf("mov %s, %s\n", REG_MEM, REG);
	}

	else {
		if (DIR) printf("mov %s, [%s]\n", REG, REG_MEM);
		else printf("mov [%s], %s\n", REG_MEM, REG);
	}
}

void ParseCopyImmediateToRegister(byte first) {
	byte WIDTH = (first & 0b00001000) >> 3;
	char* REG = DecodeRegister(first & 0b00000111, WIDTH);

	if (WIDTH == 0) printf("mov %s, %d\n", REG, ReadByte());
	else if (WIDTH == 1) printf("mov %s, %hi\n", REG, ReadWord());
}

void Disassemble() {
	byte first;

	while(1) {
		first = ReadByte();

		byte OPCODE = (first & 0b11110000) >> 4;
		switch (OPCODE) {
			case 0b1011: ParseCopyImmediateToRegister(first); continue;
		}
		
		OPCODE = (first & 0b11111100) >> 2;
		switch(OPCODE) {
			case 0b100010: ParseCopyRegisterMemoryToFromRegister(first); continue;;
		}
	}
}

int main(int argc, char* argv[]) {

	if (argc != 2) {
		printf("Bad Argument!\n"); exit(-1);
	}

	printf("; %s\n", argv[1]);
	printf("bits 16\n");

	file = fopen(argv[1], "rb");

	if (file == NULL) {
		printf("File Error!\n"); exit(-1);
	}

	Disassemble();

	fclose(file);
	return 0;
}
