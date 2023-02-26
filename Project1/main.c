#define _CRT_SECURE_NO_DEPRECATE
#include "headers.h"

#define INPUT_BUF_SIZE 60 // Size of the buffer used to read records from input file
#define	SEARCH_BUF_SIZE 25 // Size of the buffer used to read records from search file

struct student createStudent(char record[]);
void processInputFile(struct student* hashTable[], char* filename);
void processSearchFile(struct student* hashTable[], char* filename);
bool testRecord(char record[]);

int main(int argc, char* argv[]) 
{
	if (argv[1] == NULL) {
		displayError(CMD, "");
		exit(0);
	}
	char* fileName = argv[1];
	struct student* hashTable[35];
	initializeTable(&hashTable);
	processInputFile(&hashTable, fileName);
	displayHashTable(hashTable);
}

struct student createStudent(char record[])
{
	// Do not forget to use the following functions:
	// strtok() - splits a string into separate parts
	// strcpy() - copies string data from one variable to another
	// strtol() - converts a string to its integer value
	// atof() 	- converts a string to its double value

	struct student newStudent = {0};

	char delim[] = ",";
	char* name;
	char* number;
	char* email;
	char* credits;
	char* gpa;
	int num;
	double grade = 0.0;

	name = strtok(record, delim);		//get name
	number = strtok(NULL, delim);		//get number
	email = strtok(NULL, delim);		//get email
	credits = strtok(NULL, delim);		//get credits
	gpa = strtok(NULL, delim);			//get gpa
	num = strtol(credits, NULL, 10);	//convert string to int
	if (gpa != NULL) {
		grade = atof(gpa);				//convert string to double
	}
	else {
		displayError(CORRUPT, record);
	}

	strcpy(newStudent.name, name);		//copy name
	strcpy(newStudent.number, number);	//copy number
	strcpy(newStudent.email, email);	//copy email
	newStudent.credits = num;			//copy credits
	newStudent.gpa = grade;				//copy gpa

	return newStudent;
}

void processInputFile(struct student* hashTable[], char* filename)
{
	// Do not forget to use the following functions:
	// memset() - fill the buffer array with the specified character

	FILE* filepointer;
	char buffer[INPUT_BUF_SIZE];
	struct student tempStudent;
	
	filepointer = fopen(filename, "r");

	if (filepointer == NULL) {
		displayError(FNF, "");
		exit(0);
	}

	while (fgets(buffer, INPUT_BUF_SIZE, filepointer) != NULL) 
	{
		if (testRecord(buffer)) {
			tempStudent = createStudent(buffer);
			//displayRecord(tempStudent, 0);

			insertStudent(hashTable, &tempStudent);
		}
		memset(buffer, NULL, INPUT_BUF_SIZE);
	}

	fclose(filepointer);
}

void processSearchFile(struct student* hashTable[], char* filename)
{
	// Do not forget to use the following functions:
	// strtok() - splits a string into separate parts
	// memset() - fill the buffer array with the specified character
}

bool testRecord(char record[])
{

	if (record[0] == '\10') {
		displayError(BLANK, record);
		return false;
	}
	else if (strlen(record) < 35) {
		displayError(SHORT, record);
		return false;
	}
	else if (strstr(record, ",") == NULL) {
		displayError(CORRUPT, record);
		return false;
	}
	else {
		return true;
	}
}
