#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "util.h"

typedef unsigned char byte;

void read_byte(FILE* file, char* value) {
	fread(value, sizeof(byte), 1, file);
	
	if (feof(file)) {
		printf("; EOF!\n");
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

		byte OPCODE = (first & 0b11110000) >> 4;

		if (OPCODE == 0b1011) {
			// Immediate to register

			byte WIDTH = (first & 0b00001000) >> 3;
			char* REG = decode_register(first & 0b00000111, WIDTH);

			if (WIDTH == 0) {
				char DATA;
				read_byte(file, &DATA);

				printf("mov %s, %d\n", REG, DATA);
			}
			
			else if (WIDTH == 1) {
				char DATA[2];

				read_byte(file, &DATA[0]);
				read_byte(file, &DATA[1]);

				short VAL = *((short*) &DATA);
				printf("mov %s, %hi\n", REG, VAL);
			}

			continue;
		}
		
		OPCODE = (first & 0b11111100) >> 2;

		switch(OPCODE) {

			// Copy from R to R
			// Copy from R to M
			// Copy from M to R

			case 0b100010: {
				byte second;
				read_byte(file, &second);

				byte MOD = second >> 6;
				byte WIDTH = first & 0b00000001;
				byte DIR = first >> 1 & 0b00000001;


				// Register Mode (No Displacement)

				if (MOD == 0b11) {
					char* reg1 = decode_register((second >> 3) & 0b00000111, WIDTH);
					char* reg2 = decode_register(second & 0b00000111, WIDTH);

					if (DIR == 0)
						printf("mov %s, %s\n", reg2, reg1);
					else if (DIR == 1)
						printf("mov %s, %s\n", reg1, reg2);
				}

				// Memory Mode (No Displacement)

				else if (MOD == 0b00) {
					char* REG = decode_register((second >> 3) & 0b00000111, WIDTH);
					char* MEM = (char*) malloc(20);

					switch (second & 0b00000111) {
						case 0b000:
							MEM = "bx + si";
							break;
						case 0b001:
							MEM = "bx + di";
							break;
						case 0b010:
							MEM = "bp + si";
							break;
						case 0b011:
							MEM = "bp + di";
							break;
						case 0b100:
							MEM = "si";
							break;
						case 0b101:
							MEM = "di";
							break;
						case 0b110:
							MEM = "DIRECT ADDRESS?";
							break;
						case 0b111:
							MEM = "bx";
							break;
					}

					if (DIR == 0)
						printf("mov [%s], %s\n", MEM, REG);
					else if (DIR == 1)
						printf("mov %s, [%s]\n", REG, MEM);
				}

				// Memory Mode (8-bit Displacement)

				else if (MOD == 0b001) {
					char* REG = decode_register((second >> 3) & 0b00000111, WIDTH);
 					char* MEM = (char*) malloc(20);

					char DISP;


					switch (second & 0b00000111) {
						case 0b110:

							// BP + D8

							read_byte(file, &DISP);

							if (DISP > 0) sprintf(MEM, "bp + %d", DISP);
							else MEM = "bp";

							break;

						case 0b000:

							// BX + SI + D8

							read_byte(file, &DISP);

							if (DISP > 0) sprintf(MEM, "bx + si + %d", DISP);
							else MEM = "bx + si";

							break;
					}

					if (DIR == 0)
						printf("mov [%s], %s\n", MEM, REG);
					else if (DIR == 1)
						printf("mov %s, [%s]\n", REG, MEM);
				}

				// Memory Mode (16-bit Displacement)

				else if (MOD == 0b010) {
					char* REG = decode_register((second >> 3) & 0b00000111, WIDTH);
 					char* MEM = (char*) malloc(20);

					char DISP[2];

					switch (second & 0b00000111) {
						case 0b000:

							// BX + SI + D16

							read_byte(file, &DISP[0]);
							read_byte(file, &DISP[1]);

							short VAL = *((short*) &DISP);

							if (DISP > 0) sprintf(MEM, "bx + si + %hi", *((short*) &DISP));
							else MEM = "bx + si";

							break;
					}

					if (DIR == 0)
						printf("mov [%s], %s\n", MEM, REG);
					else if (DIR == 1)
						printf("mov %s, [%s]\n", REG, MEM);
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
