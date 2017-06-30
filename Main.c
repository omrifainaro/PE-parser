#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <stdlib.h>
#include "Utils.h"
#include "Constants.h"

//(Virtual address) - (base address) = RVA

void dumpDataDirectories(IMAGE_DATA_DIRECTORY* dirs, unsigned char* imageBase, int size){
	int i = 0;
	printf("%-24s%-12s%-12s\n", "Name", "Address", "Size");
	for (; i < size - 1; i++) {
		printf("%-24s%-12p%-6d%-6s\n", SECTION_NAMES[i], imageBase + dirs[i].VirtualAddress, dirs[i].Size, "bytes");
	}
	printf("\n");
}

void dumpStackAndHeapInfo(IMAGE_OPTIONAL_HEADER* OptionalHeader){
	printf("Size of stack reserved: %d bytes\n", OptionalHeader->SizeOfStackReserve);
	printf("Size of stack commited: %d bytes\n", OptionalHeader->SizeOfStackCommit);
	printf("Size of heap reserved: %d bytes\n", OptionalHeader->SizeOfHeapReserve);
	printf("Size of heap commited: %d bytes\n", OptionalHeader->SizeOfHeapCommit);
	printf("\n");
}

void dumpSectionData(IMAGE_SECTION_HEADER* sections, unsigned long baseOfFile, unsigned char* imageBase) {
	int i = 0;
	printf("%-12s%-24s%-24s%-24s%-16s\n", "Name", "Physical Address", "Virtual Address", "Pointer Raw Data", "Size Raw Data");
	for (; i < 10; i++) {
		printf("%-12s%-24p%-24p%-24p%-6d%-10s\n", sections[i].Name,
			baseOfFile + sections[i].Misc.PhysicalAddress, imageBase + sections[i].VirtualAddress,
			sections[i].PointerToRawData, sections[i].SizeOfRawData, "bytes");
	}
	printf("\n");
}

void dumpImportTable(IMAGE_IMPORT_DESCRIPTOR* importTableAddr, int size, unsigned char* imageBase) {
	int i = 0;
	size /= sizeof(IMAGE_IMPORT_DESCRIPTOR);
	for (; i < size; i++) {
		printf("File using dll rva of name: %p\n", importTableAddr[i].Name);
	}
}

int main(int argc, char** argv) {
	FILE* f;
	unsigned char* fileData;
	unsigned long fileSize;
	PIMAGE_DOS_HEADER dosHeader;
	PIMAGE_NT_HEADERS ntHeader;
	PIMAGE_SECTION_HEADER sections;
	unsigned long imageBase;
	unsigned long entrypoint;
	unsigned char* physicalImageBase;

	if (argc < 1) {
		printf("You must enter a file!");
		system("Pause");
		return -1;
	}

	f = fopen(argv[1], "rb");
	if (!f) {
		printf("[*] File %s not found!\n", argv[1]);
		perror("[*] System error: ");
		system("Pause");
		return -1;
	}

	fileSize = getFileSize(f);
	printf("[DEBUG] File size: %lu\n", fileSize);
	
	fileData = (char*)malloc(fileSize+1);
	fread(fileData, sizeof(char), fileSize, f);
	fileData[fileSize] = 0;

	dumpBytes(fileData, 200);

	dosHeader = fileData;
	if (dosHeader->e_magic == IMAGE_DOS_SIGNATURE) { printf("[DEBUG] Found dos header\n"); }

	ntHeader = (IMAGE_NT_HEADERS*)((unsigned char*) dosHeader + dosHeader->e_lfanew);
	if (ntHeader->Signature == PE_SIGNATURE) { printf("[DEBUG] Found PE signature\n"); }

	printf("[DEBUG] ImageBase: 0x%x\n", ntHeader->OptionalHeader.ImageBase);
	imageBase = ntHeader->OptionalHeader.ImageBase;
	entrypoint = imageBase + ntHeader->OptionalHeader.AddressOfEntryPoint;
	physicalImageBase = (unsigned char*)&ntHeader->OptionalHeader.ImageBase;
	printf("[*] Address of entry point: 0x%x\n", entrypoint);
	
	printf("[DEBUG] Number of sections: %d\n", ntHeader->FileHeader.NumberOfSections);
	printf("[DEBUG] Pointer to symbol table: %p\n", imageBase +  ntHeader->FileHeader.PointerToSymbolTable);
	printf("[DEBUG] Number of symbols: %d\n", ntHeader->FileHeader.NumberOfSymbols);
	printf("[DEBUG] .text section size: %d bytes\n", ntHeader->OptionalHeader.SizeOfCode);
	printf("[DEBUG] Intitialized data size: %d bytes\n", ntHeader->OptionalHeader.SizeOfInitializedData);
	printf("[DEBUG] Intitialized data size: %d bytes\n", ntHeader->OptionalHeader.SizeOfUninitializedData);

	printf("[DEBUG] Code starts: %p\n", imageBase + ntHeader->OptionalHeader.BaseOfCode);
	printf("[DEBUG] Data starts: %p\n", imageBase + ntHeader->OptionalHeader.BaseOfData);

	sections = (PIMAGE_SECTION_HEADER)((unsigned char*) ntHeader + FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader) + ntHeader->FileHeader.SizeOfOptionalHeader);

	dumpStackAndHeapInfo(&ntHeader->OptionalHeader);
	dumpDataDirectories(ntHeader->OptionalHeader.DataDirectory, physicalImageBase, ntHeader->OptionalHeader.NumberOfRvaAndSizes);
	dumpSectionData(sections, physicalImageBase, imageBase);

	dumpImportTable(physicalImageBase + ntHeader->OptionalHeader.DataDirectory[1].VirtualAddress,
		ntHeader->OptionalHeader.DataDirectory[1].Size, physicalImageBase);

	system("Pause");
	free(fileData);
	return 0;
}