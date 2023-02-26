#include "headers.h"

#define COMMENT 35
#define INDEX_STRING ",X"
#define INPUT_BUF_SIZE 60
#define NEW_LINE 10
#define SPACE 32
#define SYMBOL_TABLE_SIZE 100

// Pass 2 constants
#define BLANK_INSTRUCTION 0x000000
#define OPCODE_MULTIPLIER 0x10000
#define OUTPUT_BUF_SIZE 70
#define RSUB_INSTRUCTION 0x4C0000
#define X_MULTIPLER 0x8000

void performPass1(struct symbol* symbolArray[], char* filename, address* addresses);
struct segment* prepareSegments(char* line);
void trim(char string[]);

// Pass 2 function prototypes
char* createFilename(char* filename, const char* extension);
void performPass2(struct symbol* symbolTable[], char* filename, address* addresses);
void resetObjectFileData(objectFileData* objectData, address* addresses);
void writeToLstFile(FILE* file, int address, segment* segments, int opcode);
void writeToObjFile(FILE* file, objectFileData data);

int main(int argc, char* argv[])
{
	// Do not modify this statement
	address addresses = { 0x00, 0x00, 0x00 };

	if (argv[1] == NULL) {
		displayError(MISSING_COMMAND_LINE_ARGUMENTS, "");
		exit(0);
	}
	char* fileName = argv[1];

	symbol* symbolTable[SYMBOL_TABLE_SIZE];
	initializeSymbolTable(&symbolTable);

//	printf("Symbol Table Log\n");
//	printf("----------------\n");
	performPass1(&symbolTable, fileName, &addresses);

//	displaySymbolTable(symbolTable);
//	printf("\nAssembly Summary\n");
//	printf("----------------\n");
//	printf("Starting Address: 0x%X\n", addresses.start);
//	printf("Ending Address: 0x%X\n", addresses.current);
//	printf("Program Size in Bytes: %d\n", addresses.current - addresses.start);

	performPass2(&symbolTable, fileName, &addresses);
}

// Do Not Modify
char* createFilename(char* filename, const char* extension)
{
	char* temp = (char*)malloc(sizeof(char) * strlen(filename) + 1);
	char* temp1 = (char*)malloc(sizeof(char) * strlen(filename) + 1);

	strcpy(temp1, filename);
	strcpy(temp, strtok(temp1, "."));
	strcat(temp, extension);
	return temp;
}

void performPass1(struct symbol* symbolTable[], char* filename, address* addresses)
{
	FILE* filepointer;
	char buffer[INPUT_BUF_SIZE];
	segment* lineSegment;
	int memory;

	filepointer = fopen(filename, "r");

	if (filepointer == NULL) {
		displayError(FILE_NOT_FOUND, filename);
		exit(0);
	}

	while (fgets(buffer, INPUT_BUF_SIZE, filepointer) != NULL)
	{
		if (addresses->current > 0x8000) {
			char e[10];
			snprintf(e, 10, "%X", addresses->current);
			displayError(OUT_OF_MEMORY, e);
			exit(0);
		}

		if (buffer[0] < 32) {					//32 ASCII is the first valid character
			displayError(BLANK_RECORD, "");
			exit(0);
		}
		else if (buffer[0] == 35) {				//35 is the ASCII Val for '#'
			continue;
		}

		lineSegment = prepareSegments(buffer);		//Segment the line

		//Test the First Segment
		if (!isDirective(lineSegment->first) == 0) {						//Test if Symbol is a Directive
			displayError(ILLEGAL_SYMBOL, lineSegment->first);
			exit(0);
		}
		else if (isOpcode(lineSegment->first)) {							//Test if Symbol is an Opcode
			displayError(ILLEGAL_SYMBOL, lineSegment->first);
			exit(0);
		}

		//Test the Second Segment
		if (isDirective(lineSegment->second) > 0) {
			if (isStartDirective(isDirective(lineSegment->second))) {
				addresses->start = strtol(lineSegment->third, NULL, 16);		//Store Start Address
				addresses->current = addresses->start;
			}
			else {
				memory = getMemoryAmount(isDirective(lineSegment->second), lineSegment->third);		//Find Memory Amount From Third Segment
				addresses->increment = memory;
			}
		}
		else if (isOpcode(lineSegment->second)) {
			addresses->increment = 0x3;					//Always 3 for Opcodes
		}
		else {
			displayError(ILLEGAL_OPCODE_DIRECTIVE, lineSegment->second);
			exit(0);
		}

		//Test First Segment for a Symbol
		if (strlen(lineSegment->first) > 0) {
			insertSymbol(symbolTable, lineSegment->first, addresses->current);		//Store Symbol in Table
		}

		addresses->current += addresses->increment;		//Increment Current Address

		memset(buffer, NULL, INPUT_BUF_SIZE);
	}

	fclose(filepointer);
}

// To implement Pass 2 of the assembler for Project 3,
// Add the following function to your existing Project 2 code
void performPass2(struct symbol* symbolTable[], char* filename, address* addresses)
{
	// Do not modify this statement
	objectFileData objectData = { 0, { 0x0 }, { "\0" }, 0, 0x0, 0, { 0 }, 0, '\0', 0x0 };

	// Your code should start here
	char buffer[INPUT_BUF_SIZE];
	FILE* filepointer;
	segment* lineSegment;
	int value = 0x0;
	int numBytes = 0x0;
	int opVal = 0x0;
	char tempString[100] = "";
	int symAddr = 0x0;
	FILE* obj;
	FILE* lst;
	char* objFile = createFilename(filename, ".obj");
	char* lstFile = createFilename(filename, ".lst");
	obj = fopen(objFile, "w");
	lst = fopen(lstFile, "w");
	int iter = 0;

	filepointer = fopen(filename, "r");

	if (filepointer == NULL) {
		displayError(FILE_NOT_FOUND, filename);
		exit(0);
	}

	while (fgets(buffer, INPUT_BUF_SIZE, filepointer) != NULL) 
	{
		if (buffer[0] == 35) {
			continue;
		}

		lineSegment = prepareSegments(buffer);
		objectData.recordType = 'T';

		// DIRECTIVES
		if (isDirective(lineSegment->second) > 0) {
			if (isStartDirective(isDirective(lineSegment->second))) {
				objectData.recordType = 'H';
				strcpy(objectData.programName, lineSegment->first);
				objectData.startAddress = addresses->start;
				objectData.recordAddress = addresses->start;
				objectData.programSize = addresses->current - addresses->start;
				addresses->current = addresses->start;
				writeToObjFile(obj, objectData);
				writeToLstFile(lst, addresses->current, lineSegment, BLANK_INSTRUCTION);
			}
			else if (isEndDirective(isDirective(lineSegment->second))) {
				if (objectData.recordByteCount > 0) {
					writeToObjFile(obj, objectData);
					resetObjectFileData(&objectData, addresses);
				}
				objectData.recordType = 'E';
				writeToObjFile(obj, objectData);
				writeToLstFile(lst, addresses->current, lineSegment, BLANK_INSTRUCTION);
			}
			else if (isReserveDirective(isDirective(lineSegment->second))) {
				if (objectData.recordByteCount > 0) {
					writeToObjFile(obj, objectData);
					resetObjectFileData(&objectData, addresses);
				}
				writeToLstFile(lst, addresses->current, lineSegment, BLANK_INSTRUCTION);
				addresses->increment = getMemoryAmount(isDirective(lineSegment->second), lineSegment->third);
				objectData.recordAddress += addresses->increment;
			}
			else if (isDataDirective(isDirective(lineSegment->second))) {
				addresses->increment = getMemoryAmount(isDirective(lineSegment->second), lineSegment->third);
				if (objectData.recordByteCount > MAX_RECORD_BYTE_COUNT - addresses->increment) {
					writeToObjFile(obj, objectData);
					resetObjectFileData(&objectData, addresses);
				}
				else {
					numBytes = addresses->increment;
					value = getByteWordValue(isDirective(lineSegment->second), lineSegment->third);
					objectData.recordEntryCount++;
					objectData.recordByteCount += addresses->increment;
					writeToLstFile(lst, addresses->current, lineSegment, value);
				}
			}
		}

		//OPCODES
		if (isOpcode(lineSegment->second)) {
			if (objectData.recordByteCount > MAX_RECORD_BYTE_COUNT - 3) {
				writeToObjFile(obj, objectData);
				resetObjectFileData(&objectData, addresses);
				iter = 0;
			}
			opVal = getOpcodeValue(lineSegment->second);
			opVal = opVal * OPCODE_MULTIPLIER;
			if (strstr(lineSegment->third, ",X") != NULL) {
				opVal = opVal + X_MULTIPLER;
				strcpy(tempString, lineSegment->third);
				tempString[strlen(tempString) - 1] = '\0';
				tempString[strlen(tempString) - 1] = '\0';
				symAddr = getSymbolAddress(symbolTable, tempString);
				opVal = opVal + symAddr;
			}
			else if (getOpcodeValue(lineSegment->second) != 0x4C) {
				if (strlen(lineSegment->third) > 0) {
					if (getSymbolAddress(symbolTable, lineSegment->third) == -0x1) {
						displayError(UNDEFINED_SYMBOL, lineSegment->third);
						exit(0);
					}
					symAddr = getSymbolAddress(symbolTable, lineSegment->third);
					opVal = opVal + symAddr;
				}
			}
			objectData.recordEntries[iter].numBytes = 3;
			objectData.recordEntries[iter].value = opVal;
			objectData.recordEntryCount++;
			iter++;
			objectData.recordByteCount += 3;
			writeToLstFile(lst, addresses->current, lineSegment, opVal);
			addresses->increment = 3;
		}
		addresses->current += addresses->increment;
		memset(buffer, NULL, INPUT_BUF_SIZE);
	}

	fclose(filepointer);
	fclose(obj);
	fclose(lst);
}

segment* prepareSegments(char* statement)
{
	struct segment* temp = (segment*)malloc(sizeof(segment));
	strncpy(temp->first, statement, SEGMENT_SIZE - 1);
	strncpy(temp->second, statement + SEGMENT_SIZE - 1, SEGMENT_SIZE - 1);
	strncpy(temp->third, statement + (SEGMENT_SIZE - 1) * 2, SEGMENT_SIZE - 1);

	trim(temp->first);
	trim(temp->second);
	trim(temp->third);
	return temp;
}

// To implement Pass 2 of the assembler for Project 3,
// Add the following function to your existing Project 2 code
void resetObjectFileData(objectFileData* objectData, address* addresses)
{
	objectData->recordAddress = addresses->current;
	objectData->recordByteCount = 0;
	objectData->recordEntryCount = 0;
}

// To implement Pass 2 of the assembler for Project 3,
// Use the following function to replace the function
// In your existing Project 2 code
void trim(char value[])
{
	for (int x = 0; x < SEGMENT_SIZE; x++)
	{
		if (value[x] == SPACE || x == (SEGMENT_SIZE - 1))
		{
			value[x] = '\0';
		}
	}
}

// To implement Pass 2 of the assembler for Project 3,
// Add the following function to your existing Project 2 code
void writeToLstFile(FILE* file, int address, segment* segments, int opcode)
{
	if (isStartDirective(isDirective(segments->second)) || isReserveDirective(isDirective(segments->second))) {
		fprintf(file, "%.4X    ", address);
		fprintf(file, "%-8s", segments->first);
		fprintf(file, "%-8s", segments->second);
		fprintf(file, "%-8s", segments->third);
		fprintf(file, "\n");
	}
	else if (isEndDirective(isDirective(segments->second))) {
		fprintf(file, "%X    ", address);
		fprintf(file, "%-8s", segments->first);
		fprintf(file, "%-8s", segments->second);
		fprintf(file, "%-8s", segments->third);
	}
	else if (isDataDirective(isDirective(segments->second))) {
		fprintf(file, "%X    ", address);
		fprintf(file, "%-8s", segments->first);
		fprintf(file, "%-8s", segments->second);
		fprintf(file, "%-8s", segments->third);
		if (isDirective(segments->second) == 1) {
			if (strstr(segments->third, "X") != NULL) {
				fprintf(file, "    %.2X", opcode);
			}
			else if (strstr(segments->third, "C") != NULL) {
				fprintf(file, "    %X", opcode);
			}
		}
		else if (isDirective(segments->second) == 6) {
			fprintf(file, "    %.6X", opcode);
		}
		fprintf(file, "\n");
	}
	else if (isOpcode(segments->second)) {
		fprintf(file, "%.4X    ", address);
		fprintf(file, "%-8s", segments->first);
		fprintf(file, "%-8s", segments->second);
		fprintf(file, "%-8s", segments->third);
		fprintf(file, "    %.6X", opcode);
		fprintf(file, "\n");
	}
}

// To implement Pass 2 of the assembler for Project 3,
// Add the following function to your existing Project 2 code
void writeToObjFile(FILE* file, objectFileData fileData)
{	
	if (fileData.recordType == 'H') {
		fprintf(file, "H");
		fprintf(file, "%-6s", fileData.programName);
		fprintf(file, "%.6X", fileData.startAddress);
		fprintf(file, "%.6X", fileData.programSize);
		fprintf(file, "\n");
	}
	else if (fileData.recordType == 'T') {
		fprintf(file, "T");
		fprintf(file, "%.6X", fileData.recordAddress);
		fprintf(file, "%.2X", fileData.recordByteCount);
		for (int i = 0; i < fileData.recordEntryCount; i++) {
			fprintf(file, "%.6X", fileData.recordEntries[i].value);
		}
		fprintf(file, "\n");
	}
	else if (fileData.recordType == 'E') {
		fprintf(file, "E");
		fprintf(file, "%.6X", fileData.startAddress);
	}
}
