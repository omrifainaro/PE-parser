#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>

unsigned long getFileSize(FILE* f) {
	if (!f) {
		return 0;
	}
	long fileSize;
	fseek(f, 0, SEEK_END);
	fileSize = ftell(f);
	fseek(f, 0, SEEK_SET);
	return fileSize;
}

void dumpBytes(unsigned char* buffer, int numBytes) {
	if (!buffer) {
		printf("NULL");
		return;
	}
	int i = 0;
	for (; i < numBytes; i++) {
		printf("%02x ", buffer[i]);
	}
	printf("\n");
}

void printAsAscii(unsigned char* buffer, int numBytes) {
	int i = 0;
	for (; i < numBytes; i++) {
		printf("%c", buffer[i]);
	}
}

void reverseBytes(char* buffer, int byteSize, int count) {
	int i = 0, j = byteSize*count;
	char* temp = malloc(byteSize);
	for (; i < j; i += byteSize, j -= byteSize) {
		memcpy(temp, (buffer + i), byteSize);
		memcpy(buffer + i, (buffer + j - byteSize), byteSize);
		memcpy((buffer + j - byteSize), temp, byteSize);
	}
}

#endif
