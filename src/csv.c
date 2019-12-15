#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "csv.h"
CSV *CSVreadFile(CSV *csvp, FILE *fp, short *error)
{
	csvp->table = NULL;
	char lastChar = '\0';
	char readChar = '\0';

	csvp->rows = 1;
	csvp->cols = 1;
	unsigned short currentColumn = 1;
	unsigned long cells = 1;
	unsigned short quotesInRow = 0;

	bool inQuote = false;
	void *temp = NULL;

	*error = CSVNoError;

	do
	{
		lastChar = readChar;
		readChar = fgetc(fp);

		if (readChar == EOF && inQuote == true)
		{
			*error = CSVFileIsCorrupted;
			return NULL;
		}

		if (readChar == EOF && cells == 1)
		{
			*error = CSVEmptyFile;
			csvp->rows = 0;
			csvp->cols = 0;
			csvp->table = NULL;
			return NULL;
		}

		if (csvp->table == NULL)
		{
			temp = (String ***)calloc(1, sizeof(String **));
			if (temp == NULL)
			{
				*error = CSVAllocationError;
				return NULL;
			}

			csvp->table = (String ***)temp;

			temp = (String **)calloc(1, sizeof(String *));
			if (temp == NULL)
			{
				*error = CSVAllocationError;
				return NULL;
			}

			csvp->table[0] = (String **)temp;

			temp = StrCreateEmpty();
			if (temp == NULL)
			{
				*error = CSVAllocationError;
				return NULL;
			}

			csvp->table[0][0] = temp;
		}

		if (inQuote)
		{
			if (readChar == '"')
			{
				quotesInRow++;
				while (true)
				{
					char c = fgetc(fp);
					if (c == '"')
					{
						quotesInRow++;
						continue;
					}
					else if (c == EOF && quotesInRow % 2 == 0)
					{
						for (unsigned short i = 0; i < currentColumn; i++)
						{
							free(csvp->table[csvp->rows - 1][i]->c_str);
							free(csvp->table[csvp->rows - 1][i]);
						}

						free(csvp->table[csvp->rows - 1]);
						csvp->rows--;
						cells--;
						currentColumn = csvp->cols;
						*error = CSVFileIsCorrupted;
						return NULL;
					}
					else if (c == EOF && quotesInRow % 2 == 1)
					{
						if (currentColumn != csvp->cols)
						{
							for (unsigned short i = 0; i < currentColumn; i++)
							{
								free(csvp->table[csvp->rows - 1][i]->c_str);
								free(csvp->table[csvp->rows - 1][i]);
							}

							free(csvp->table[csvp->rows - 1]);
							csvp->rows--;
							cells--;
							currentColumn = csvp->cols;
						}
						else
						{
							for (unsigned short i = 0; i < quotesInRow; i++)
							{
								temp = StrAppend(csvp->table[csvp->rows - 1][currentColumn - 1], '"');
								if (temp == NULL)
								{
									*error = CSVAllocationError;
									return NULL;
								}
							}
							inQuote = false;
							lastChar = '"';
							readChar = c;
							fseek(fp, -1, SEEK_CUR);
							quotesInRow = 0;
							break;
						}
					}
					else
					{
						if (quotesInRow % 2 == 1)
							inQuote = false;

						for (unsigned short i = 0; i < quotesInRow; i++)
						{
							temp = StrAppend(csvp->table[csvp->rows - 1][currentColumn - 1], '"');
							if (temp == NULL)
							{
								*error = CSVAllocationError;
								return NULL;
							}
						}

						lastChar = '"';
						readChar = c;
						fseek(fp, -1, SEEK_CUR);
						quotesInRow = 0;
						break;
					}
				}
			}
			else if (readChar == EOF)
			{
				*error = CSVFileIsCorrupted;
				return NULL;
			}
			else
			{
				temp = StrAppend(csvp->table[csvp->rows - 1][currentColumn - 1], readChar);
				if (temp == NULL)
				{
					*error = CSVAllocationError;
					return NULL;
				}
			}
		}
		else
		{
			switch (readChar)
			{
			case '\0':
			case '\r':
				break;

			case ',':
				cells++;
				temp = (String **)realloc(csvp->table[csvp->rows - 1], (currentColumn + 1) * sizeof(String *));

				if (temp == NULL)
				{
					*error = CSVAllocationError;
					return NULL;
				}

				csvp->table[csvp->rows - 1] = temp;

				temp = StrCreateEmpty();
				if (temp == NULL)
				{
					*error = CSVAllocationError;
					return NULL;
				}

				csvp->table[csvp->rows - 1][currentColumn] = temp;
				currentColumn++;
				break;

			case '"':
				temp = StrAppend(csvp->table[csvp->rows - 1][currentColumn - 1], readChar);
				if (temp == NULL)
				{
					*error = CSVAllocationError;
					return NULL;
				}

				inQuote = true;
				break;

			case '\n':
				cells++;
				csvp->rows++;
				temp = (String ***)realloc(csvp->table, csvp->rows * sizeof(String **));
				if (temp == NULL)
				{
					*error = CSVAllocationError;
					return NULL;
				}

				csvp->table = temp;

				temp = (String **)calloc(1, sizeof(String *));
				if (temp == NULL)
				{
					*error = CSVAllocationError;
					return NULL;
				}

				csvp->table[csvp->rows - 1] = temp;

				temp = StrCreateEmpty();
				if (temp == NULL)
				{
					*error = CSVAllocationError;
					return NULL;
				}

				csvp->table[csvp->rows - 1][0] = temp;

				if (csvp->rows == 2)
					csvp->cols = currentColumn;
				else if (currentColumn != csvp->cols)
				{
					for (unsigned short i = 0; i < currentColumn; i++)
					{
						free(csvp->table[csvp->rows - 1][i]->c_str);
						free(csvp->table[csvp->rows - 1][i]);
					}

					free(csvp->table[csvp->rows - 1]);
					csvp->rows--;
					cells--;
					currentColumn = csvp->cols;
					*error = CSVFileIsCorrupted;
					return NULL;
				}

				currentColumn = 1;
				break;

			case EOF:
				if (currentColumn != csvp->cols && currentColumn != 1)
				{
					*error = CSVFileIsCorrupted;
					return NULL;
				}
				else if (lastChar == '\n')
				{
					for (unsigned short i = 0; i < currentColumn; i++)
					{
						free(csvp->table[csvp->rows - 1][i]->c_str);
						free(csvp->table[csvp->rows - 1][i]);
					}

					free(csvp->table[csvp->rows - 1]);
					csvp->rows--;
					cells--;
					currentColumn = csvp->cols;
				}
				break;

			default:
				temp = StrAppend(csvp->table[csvp->rows - 1][currentColumn - 1], readChar);

				if (temp == NULL)
				{
					*error = CSVAllocationError;
					return NULL;
				}

				break;
			}
		}
	} while (readChar != EOF);

	return csvp;
}

unsigned long CSVgetRows(CSV *csvp)
{
	return csvp->rows;
}

unsigned short CSVgetCols(CSV *csvp)
{
	return csvp->cols;
}

String *CSVgetCell(CSV *csvp, unsigned long row, unsigned short col, short *error)
{
	if (row < CSVgetRows(csvp) && col < CSVgetCols(csvp))
	{
		*error = CSVNoError;
		return csvp->table[row][col];
	}
	else
	{
		*error = CSVIndexError;
		return NULL;
	}
}

char *CSVprocessString(String str)
{
	unsigned short quotesInRow = 0;
	bool inQuote = false;
	char *result = (char *)calloc(str._length, sizeof(char));
	if (result == NULL)
		return NULL;

	char lastChar = '\0';
	char readChar = '\0';
	for (unsigned short i = 0, r = 0; i < str._length; i++)
	{
		lastChar = readChar;
		readChar = str.c_str[i];
		if (inQuote == false)
		{
			if (readChar == '"')
				inQuote = true;
			else
			{
				result[r] = readChar;
				r++;
			}
		}
		else
		{
			if (readChar == '"')
				quotesInRow++;
			else
			{
				if (lastChar == '"')
				{
					for (unsigned short j = 0; j < quotesInRow / 2; j++, r++)
						result[r] = '"';
					if (quotesInRow % 2 == 1)
						inQuote = false;

					quotesInRow = 0;

					result[r] = readChar;
					r++;
				}
				else
				{
					result[r] = readChar;
					r++;
				}
			}
		}
	}

	if (strlen(result) < str._length - 1)
	{
		char *temp = realloc(result, strlen(result) + 1);
		if (temp == NULL)
			return NULL;
		result = temp;
	}
	return result;
}

void CSVprintRow(CSV *csvp, unsigned long row, short *error)
{
	if (row >= CSVgetRows(csvp))
	{
		*error = CSVIndexError;
		return CSVIndexError;
	}
	char *colName = NULL;
	char *cellVal = NULL;
	printf("#ROW: %lu\n", row);
	for (unsigned short col = 0, cols = CSVgetCols(csvp); col < cols; col++)
	{
		colName = CSVprocessString(*CSVgetCell(csvp, 0, col, error));
		cellVal = CSVprocessString(*CSVgetCell(csvp, row, col, error));
		printf("%s: %s\n", colName, cellVal);
		free(colName);
		free(cellVal);
	}
	*error = CSVNoError;
}

void CSVprintInfo(CSV *csvp)
{
	printf("CSV file contains %lu rows %hu cols each.\n", CSVgetRows(csvp) - 1, CSVgetCols(csvp));
}

void CSVclean(CSV *csvp)
{
	for (unsigned long i = 0, rows = CSVgetRows(csvp); i < rows; i++)
	{
		for (unsigned short j = 0, cols = CSVgetCols(csvp); j < cols; j++)
		{
			free(csvp->table[i][j]->c_str);
			free(csvp->table[i][j]);
		}

		free(csvp->table[i]);
	}

	free(csvp->table);
}

void CSVprintErrorMsg(short errorCode)
{
	switch (errorCode)
	{
	case CSVAllocationError:
		printf("There was a memory allocation problem. Perhaps your file is too big?\n");
		break;

	case CSVIndexError:
		printf("Provided row number is too big!\n");
		break;

	case CSVEmptyFile:
		printf("Provided file is empty!\n");
		break;

	case CSVFileIsCorrupted:
		printf("Provided file is corrupted!\n");
		break;

	default:
		break;
	}
}