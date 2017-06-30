#pragma once

#define PE_SIGNATURE 0x4550

char* SECTION_NAMES[15] = { "Export Table", "Import Table", "Resource Table",
"Exception Table", "Certificate Table", "Base Relocation Table", "DEBUG", "Architecture Table", 
"Global Pointer", "Tls Table", "Load Config Table", "Bound Import", "Import Addresss Table", 
"Delay Import Descriptor", "CLR Runtime Header"};
