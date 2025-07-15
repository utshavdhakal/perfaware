#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "util.h"

typedef unsigned char byte;

void read_byte(FILE* file, byte* value) {
	fread(value, sizeof(byte), 1, file);
	
	if (feof(file)) {
		exit(-1);
	}

	if (ferror(file)) {
		printf("File Read Error!\n");
		exit(-1);
	}

}

char* decode_register(byte reg, byte width) {
	switch(reg) {
		case 0b000:
			return width == 0 ? "al" : "ax";
		case 0b001:
			return width == 0 ? "cl" : "cx";
		case 0b010:
			return width == 0 ? "dl" : "dx";
		case 0b011:
			return width == 0 ? "bl" : "bx";
		case 0b100:
			return width == 0 ? "ah" : "sp";
		case 0b101:
			return width == 0 ? "ch" : "bp";
		case 0b110:
			return width == 0 ? "dh" : "si";
		case 0b111:
			return width == 0 ? "bh" : "di";
	}
}

void disassemble(FILE* file) {
	byte first;

	while(1) {
		read_byte(file, &first);
		
		byte OPCODE = (first & 0b11111100) >> 2;
		switch(OPCODE) {

			// Copy from R to R
			// Copy from R to M
			// Copy from M to R

			case 0b100010: {
				byte second;
				read_byte(file, &second);

				byte MOD = second >> 6;
				byte WIDTH = first & 0b00000001;

				// Register to Register

				if (MOD == 0b11) {
					char* reg1 = decode_register((second >> 3) & 0b00000111, WIDTH);
					char* reg2 = decode_register(second & 0b00000111, WIDTH);

					byte DIR = first >> 1 & 0b00000001;

					if (DIR == 0) {
						printf("mov %s, %s\n", reg2, reg1);
					}

					else if (DIR == 1) {
						printf("mov %s, %s\n", reg1, reg2);
					}
				}
			}
				
		}
	}
}

int main(int argc, char* argv[]) {

	if (argc != 2) {
		printf("Bad Argument!\n");
		exit(-1);
	}

	printf("; %s\n", argv[1]);
	printf("bits 16\n");

	FILE* file = fopen(argv[1], "rb");

	if (file == NULL) {
		printf("File Error!\n");
		exit(-1);
	}

	disassemble(file);

	fclose(file);
	return 0;
}
