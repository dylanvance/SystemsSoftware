#include "headers.h"

#define COMMENT 35
#define INPUT_BUF_SIZE 60
#define NEW_LINE 10
#define SPACE 32
#define SYMBOL_TABLE_SIZE 100

void performPass1(struct symbol* symbolArray[], char* filename, address* addresses);
struct segment* prepareSegments(char* line);
void trim(char string[]);

int main(int argc, char* argv[])
{
	// Do not modify this statement
	address addresses = { 0x00, 0x00, 0x00 };

	// Your code should start here
	if (argv[1] == NULL) {
		displayError(MISSING_COMMAND_LINE_ARGUMENTS, "");
		exit(0);
	}
	char* fileName = argv[1];
	
	symbol* symbolTable[SYMBOL_TABLE_SIZE];			//Creation
	initializeSymbolTable(&symbolTable);			//Initialization
	
	printf("Symbol Table Log\n");
	printf("----------------\n");
	performPass1(&symbolTable, fileName, &addresses);

	displaySymbolTable(symbolTable);				//Displaytion

	printf("\nAssembly Summary\n");
	printf("----------------\n");
	printf("Starting Address: 0x%X\n", addresses.start);
	printf("Ending Address: 0x%X\n", addresses.current);
	printf("Program Size in Bytes: %d\n", addresses.current - addresses.start);
}

void performPass1(struct symbol* symbolTable[], char* filename, address* addresses)
{
	FILE* filepointer;
	char buffer[INPUT_BUF_SIZE];
	segment* lineSegment;
	int memory;

	filepointer = fopen(filename, "r");
//	filepointer = fopen("testA.sic", "r");

	if (filepointer == NULL) {
		displayError(FILE_NOT_FOUND, filename);
		exit(0);
	}

	while (fgets(buffer, INPUT_BUF_SIZE, filepointer) != NULL) 
	{
		if (addresses->current > 0x8000) {
			char e[10];
//			itoa(addresses->current, e, 16);
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
		if (strcmp(lineSegment->first, "COPY") != 0) {				//No Need to Store "COPY" Symbol
			if (strlen(lineSegment->first) > 0) {
				insertSymbol(symbolTable, lineSegment->first, addresses->current);		//Store Symbol in Table
			}
		}

		addresses->current += addresses->increment;		//Increment Current Address

		memset(buffer, NULL, INPUT_BUF_SIZE);
	}

	fclose(filepointer);
}

// Do no modify any part of this function
segment* prepareSegments(char* statement)
{
	struct segment* temp = malloc(sizeof(segment));
	strncpy(temp->first, statement, SEGMENT_SIZE - 1);
	strncpy(temp->second, statement + SEGMENT_SIZE - 1, SEGMENT_SIZE - 1);
	strncpy(temp->third, statement + (SEGMENT_SIZE - 1) * 2, SEGMENT_SIZE - 1);

	trim(temp->first);
	trim(temp->second);
	trim(temp->third);
	return temp;
}

// Do no modify any part of this function
void trim(char value[])
{
	for (int x = 0; x < SEGMENT_SIZE; x++)
	{
		if (value[x] == SPACE)
		{
			value[x] = '\0';
		}
	}
}
