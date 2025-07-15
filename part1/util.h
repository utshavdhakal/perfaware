typedef unsigned char byte;

char* binary(byte b) {
    char* result = malloc(9);
    result[8] = '\0';
    result[0] = (b & 0x80 ? '1' : '0');
    result[1] = (b & 0x40 ? '1' : '0');
    result[2] = (b & 0x20 ? '1' : '0');
    result[3] = (b & 0x10 ? '1' : '0');
    result[4] = (b & 0x08 ? '1' : '0');
    result[5] = (b & 0x04 ? '1' : '0');
    result[6] = (b & 0x02 ? '1' : '0');
    result[7] = (b & 0x01 ? '1' : '0');
    return result;
}