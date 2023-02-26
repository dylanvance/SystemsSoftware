#pragma once // Prevents the header file from loading multiple times

void displayError(int error, char* errorInfo);

enum Error {CMD, FNF, BLANK, CORRUPT, SHORT, DUPE, NAMELEN, NUMLEN, NUMFORM, EMAILLEN, EMAILFORM, CREDVAL, GPAVAL};

void displayError(int error, char* errorInfo)
{
	switch (error) 
	{
		case CMD:
			printf("ERROR: Too Few Arguments In Command Line Arguments (Missing FILE.TXT)\n");
			break;

		case FNF:
			printf("ERROR: File Not Found (%s)\n", errorInfo);
			break;

		case BLANK:
			printf("ERROR: Blank Record\n");
			break;

		case CORRUPT:
			printf("ERROR: Corrupt Record (%s)\n", errorInfo);
			break;

		case SHORT:
			printf("ERROR: Short Record (%s)\n", errorInfo);
			break;

		case DUPE:
			printf("ERROR: Duplicate Name (%s)\n", errorInfo);
			break;

		case NAMELEN:
			printf("ERROR: Name Length (%s)\n", errorInfo);
			break;

		case NUMLEN:
			printf("ERROR: Number Length Does Not Equal 9 (%s)\n", errorInfo);
			break;

		case NUMFORM:
			printf("ERROR: Number Format (%s)\n", errorInfo);
			break;

		case EMAILLEN:
			printf("ERROR: Email Length Does Not Equal 17 (%s)\n", errorInfo);
			break;

		case EMAILFORM:
			printf("ERROR: Email Foramt (%s)\n", errorInfo);
			break;

		case CREDVAL:
			printf("ERROR: Credit Value Exceeds 150 (%s)\n", errorInfo);
			break;

		case GPAVAL:
			printf("ERROR: GPA Value Exceeds 4.0 (%s)", errorInfo);
			break;

		default: 
			printf("ERROR: Uknown Error Occured\n");
			break;
	}
}