#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "stdinode.h"


long int create_indirect_pointers_block(FILE* fp, struct blockPointers* ptrs) {
	for (int i = 0; i < BLOCK_POINTERS; i++) {
		ptrs ->pointers[i] = -1;
	}
	long int address = free_block_position(fp, 1);
	fseek(fp, address, SEEK_SET);
	fwrite(ptrs, sizeof(struct blockPointers), 1, fp);
	return address;
}
