#pragma once // Prevents the header file from loading multiple times

// Constants for the Student Structure
#define EMAIL_SIZE 18
#define NAME_SIZE 24
#define NUMBER_SIZE 10

// Constants for the Hash Table
#define HASH_TABLE_SEGMENTS 7 // Used in the hashing algorithm
#define MOD_SIZE 5 // Used in the hashing algorithm
#define NEW_LINE 10 // Newline character code
#define TABLE_SIZE 35 // Number of available spaces

struct student
{
        char name[NAME_SIZE];
        char number[NUMBER_SIZE];
        char email[EMAIL_SIZE];
        int credits;
        double gpa;
};

int computerHash(char* studentName);
void displayHashTable(struct student* hashTable[]);
void displayRecord(struct student* temp, int index);
void initializeTable(struct student* hashTable[]);
void insertStudent(struct student* hashTable[], struct student* temp);
bool testEmailFormat(struct student temp);
bool testStudentData(struct student temp);

int computerHash(char* input)
{
	return strlen(input) % MOD_SIZE * HASH_TABLE_SEGMENTS;
}

void displayHashTable(struct student* hashTable[])
{
	printf("Index  Name                  Number     Email Address      Credits  GPA\n");
	printf("-----  --------------------  ---------  -----------------  -------  ---\n");
	for (int i = 0; i < TABLE_SIZE; i++) 
	{
		if (hashTable[i] != NULL) {
			displayRecord(hashTable[i], i);
		}
	/*	else {
			printf("%3d Empty Record\n", i);
		} */
	}
}

void displayRecord(struct student* temp, int index)
{
	printf("%3d	", index);
	printf("%20s ", temp->name);
	printf("%s ", temp->number);
	printf("%s ", temp->email);
	printf("%3d ", temp->credits);
	printf("%.1f", temp->gpa);
	printf("\n");
}

void initializeTable(struct student* hashTable[])
{
	for (int i = 0; i < TABLE_SIZE; i++)			//initializes entire table to NULL
	{
		hashTable[i] = NULL;
	}
}

void insertStudent(struct student* hashTable[], struct student* temp)
{
	// Do not forget to use the malloc() function to reserve space for the student structure
	// Do not forget to use the strcpy() function to store the string data in the hash table
	
	int hashIndex = computerHash(temp->name);
	struct student* pointer;

	while (true)
	{

		if (!testStudentData(*temp)) {
			break;
		} 
		
		if (hashTable[hashIndex] == NULL)
		{
			pointer = malloc(sizeof(struct student));
			hashTable[hashIndex] = pointer;
			
			strcpy(hashTable[hashIndex]->name, temp->name);
			strcpy(hashTable[hashIndex]->number, temp->number);
			strcpy(hashTable[hashIndex]->email, temp->email);
			hashTable[hashIndex]->credits = temp->credits;
			hashTable[hashIndex]->gpa = temp->gpa;
			
			//displayRecord(hashTable[hashIndex], hashIndex);
			printf("Added Student to Hash Table %20s Target: %d Actual: %d\n", hashTable[hashIndex]->name, computerHash(hashTable[hashIndex]->name), hashIndex);
			
			break;
		}
		else {
			if (strcmp(hashTable[hashIndex]->name, temp->name) == 0) {
				displayError(DUPE, temp->name);
				break;
			}
			else {
				hashIndex += 1;
			}
		}
	}
}

bool testEmailFormat(struct student temp)
{
	if (temp.email[0] != 'u') {
		return false;
	}
	else if (temp.email[9] != '@') {
		return false;
	}
	else if (temp.email[13] != '.') {
		return false;
	}
	else if (strstr(temp.email, "ufb") == NULL) {
		return false;
	}
	else if (strstr(temp.email, "edu") == NULL) {
		return false;
	}
	else {
		return true;
	}
}

bool testStudentData(struct student temp)
{
	if (strlen(temp.name) < 5 || strlen(temp.name) > 20) {
		displayError(NAMELEN, temp.name);
		return false;
	}
	else if (strlen(temp.number) != 9) {
		displayError(NUMLEN, temp.number);
		return false;
	}
	else if (temp.number[0] != 'u') {
		displayError(NUMFORM, temp.number);
		return false;
	}
	else if (strlen(temp.email) != 17) {
		displayError(EMAILLEN, temp.email);
		return false;
	}
	else if (!testEmailFormat(temp)) {
		displayError(EMAILFORM, temp.email);
		return false;
	}
	else if (temp.credits < 0 || temp.credits > 150) {
		displayError(CREDVAL, "");
		return false;
	}
	else if (temp.gpa < 0.0 || temp.gpa > 4.0) {
		displayError(GPAVAL, "");
		return false;
	} 
	else {
		return true;
	}
}
