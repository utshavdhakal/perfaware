#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef char byte;

char* decode_register(byte reg, byte width) {
	switch(reg) {
		case 0b000:
			return width == 0 ? "AL" : "AX";
		case 0b001:
			return width == 0 ? "CL" : "CX";
		case 0b010:
			return width == 0 ? "DL" : "DX";
		case 0b011:
			return width == 0 ? "BL" : "BX";
		case 0b100:
			return width == 0 ? "AH" : "SP";
		case 0b101:
			return width == 0 ? "CH" : "BP";
		case 0b110:
			return width == 0 ? "DH" : "SI";
		case 0b111:
			return width == 0 ? "BH" : "DI";
	}
}

void disassemble(byte *buffer) {
	byte opcode = (buffer[0] & 0b11111100) >> 2;
	byte dir = (buffer[0] & 0b00000010) >> 1;
	byte width = (buffer[0] & 0b00000001);
	byte mode = (buffer[1] & 0b11000000) >> 6;
	byte opr1 = (buffer[1] & 0b00111000) >> 3;
	byte opr2 = (buffer[1] & 0b00000111);

	switch(opcode) {
		
		case 0b100010:
			printf("MOV ");
	}

	switch(mode) {

		case 0b11:
			char *opr1_reg = decode_register(opr1, width);
			char *opr2_reg = decode_register(opr2, width);

			if (dir == 0) {
				printf(" %s, %s\n", opr2_reg, opr1_reg);
			} else {
				printf(" %s, %s\n", opr1_reg, opr2_reg);
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

	byte buffer[2];
	assert(fread(buffer, 2, 1, file)==1);

	disassemble(buffer);

	fclose(file);
	return 0;
}
