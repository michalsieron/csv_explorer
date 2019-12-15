#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "csv.h"

// 4K		-> printf
// 4K		-> scanf
// 552B	-> fopen (FILE *)
// 4K		-> fgetc
// 8B		-> buffer (String **)
// 8B		-> buffer[0] (String *)
// 16B	-> buffer[0][0] (String {char *, unsigned short})
// 1B		-> buffer[0][0].c_str (char)

// duża liczba zaalokowanych bajtów zwracana przez valgrind'a
// nie liczy zwolnień pamięci

short mainMenuLoop();
short fileOpsLoop();
short openFile();
void clearInputBuffer(char *input);
void printErrorMsg(short errorCode);

enum ErrorCodes
{
	NoError,
	NoSuchFile,
	FileIsCorrupted,
	InvalidCommand,
	CriticalError,
	OtherError,
};

bool isFileOpened = false;
FILE *fp;
CSV csvFile;

int main()
{
	printf("CSV explorer by Michal Sieron 2019\n");
	return mainMenuLoop();
}

short openFile()
{
	short errorCode = CSVNoError;
	char userInput[200];

	printf("Enter path to the .csv file you want to open: ");
	fflush(stdout);
	scanf("%s", userInput);
	clearInputBuffer(stdin);

	fp = fopen(userInput, "r");
	if (!fp)
	{
		return NoSuchFile;
	}
	isFileOpened = true;

	CSVreadFile(&csvFile, fp, &errorCode);
	if (errorCode != CSVNoError)
	{
		CSVprintErrorMsg(errorCode);
		CSVclean(&csvFile);
		fclose(fp);
		return OtherError;
	}
}

short mainMenuLoop()
{
	short result = NoError;
	char userInput;
	while (true)
	{
		printf("\nType letter to choose an option: \n"
					 "o - open file\n"
					 "e - exit program\n");
		printf("menu> ");
		fflush(stdout);
		scanf("%c", &userInput);
		clearInputBuffer(stdin);
		switch (userInput)
		{
		case 'o':
			result = openFile();
			if (result != NoError)
			{
				printErrorMsg(result);
				if (result == CriticalError)
					return CriticalError;
				continue;
			}

			result = fileOpsLoop();
			printErrorMsg(result);
			if (result == CriticalError)
				return CriticalError;
			break;
		case 'e':
			return NoError;
			break;

		default:
			printErrorMsg(InvalidCommand);
			break;
		}
	}
	return NoError;
}

short fileOpsLoop()
{
	short errorCode = CSVNoError;
	char userInput;
	unsigned long row = 0;
	while (true)
	{
		printf("\nType letter to choose an option: \n"
					 "p - print whole table\n"
					 "r - print one row\n"
					 "i - print file info\n"
					 "c - close opened file\n\n");
		printf("file> ");
		fflush(stdout);
		scanf("%c", &userInput);
		clearInputBuffer(stdin);
		switch (userInput)
		{
		case 'p':
			for (unsigned long i = 1, rows = CSVgetRows(&csvFile); i < rows; i++)
			{
				CSVprintRow(&csvFile, i, &errorCode);
				if (errorCode != CSVNoError)
				{
					CSVprintErrorMsg(errorCode);
					return CriticalError;
				}
				if (i < rows - 1)
					printf("\n");
			}
			break;

		case 'r':
			printf("Type row number you want to print: ");
			fflush(stdout);
			scanf("%lu", &row);
			clearInputBuffer(stdin);
			CSVprintRow(&csvFile, row, &errorCode);
			if (errorCode != CSVNoError)
			{
				CSVprintErrorMsg(errorCode);
				if (errorCode != CSVIndexError)
					return CriticalError;
			}
			break;

		case 'i':
			CSVprintInfo(&csvFile);
			break;

		case 'c':
			fclose(fp);
			CSVclean(&csvFile);
			isFileOpened = false;
			return NoError;
			break;

		default:
			printErrorMsg(InvalidCommand);
			break;
		}
	}
	return NoError;
}

void printErrorMsg(short errorCode)
{
	switch (errorCode)
	{
	case NoSuchFile:
		printf("There is no such file! Check your path.\n");
		break;

	case FileIsCorrupted:
		printf("Provided file is corrupted.\n");
		break;

	case InvalidCommand:
		printf("Invalid command!\n");
		break;

	case CriticalError:
		printf("There was a critical error! Please report it to developers!\n");
		break;

	default:
		break;
	}
}

void clearInputBuffer(char *input)
{
	if (!strchr(input, '\n'))
	{
		scanf("%*[^\n]");
		scanf("%*c");
	}
}
