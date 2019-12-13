#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "csv.h"
/*
 * Funkcja CSVreadFile wczytuje po jednym znaku z podanego pliku. Podczas 
 * jednej iteracji analizuje znak wczytany podczas poprzedniej. Dla każdego znaku
 * alokuje odpowiednią ilość pamięci
 */
CSV *CSVreadFile(CSV *csvp, FILE *fp)
{
	unsigned long totalAllocation = 0;

	String ***buffer = NULL;
	char lastChar = '\0';
	char readChar = '\0';

	unsigned long rows = 1;
	unsigned short cols = 1;
	unsigned short currentColumn = 1;
	unsigned long cells = 1;
	unsigned short quotesInRow = 0;

	bool inQuote = false;
	void *temp = NULL;

	do
	{
		lastChar = readChar;
		readChar = fgetc(fp);

		if (readChar == EOF && cells == 1)
			return NULL;

		if (buffer == NULL)
		{
			temp = (String ***)calloc(1, sizeof(String **));
			totalAllocation += sizeof(String **);
			if (temp == NULL)
				return NULL;

			buffer = (String ***)temp;

			temp = (String **)calloc(1, sizeof(String *));
			totalAllocation += sizeof(String *);
			if (temp == NULL)
				return NULL;

			buffer[0] = (String **)temp;

			temp = StrCreateEmpty();
			totalAllocation += sizeof(String);
			totalAllocation += sizeof(char);
			if (temp == NULL)
				return NULL;

			buffer[0][0] = temp;
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
					else if (c == EOF)
					{
						for (unsigned long i = 0; i < csvp->rows; i++)
						{
							for (unsigned short j = 0; j < csvp->cols; j++)
							{
								free(csvp->table[i][j]->c_str);
								free(csvp->table[i][j]);
							}

							free(csvp->table[i]);
						}

						free(csvp->table);
						return NULL;
					}
					else
					{
						if (quotesInRow % 2 == 1)
							inQuote = false;

						for (unsigned short i = 0; i < quotesInRow; i++)
						{
							temp = StrAppend(buffer[rows - 1][currentColumn - 1], '"');
							totalAllocation += (buffer[rows - 1][currentColumn - 1]->_length + 1) * sizeof(char);
							if (temp == NULL)
								return NULL;
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
				for (unsigned long i = 0; i < csvp->rows; i++)
				{
					for (unsigned short j = 0; j < csvp->cols; j++)
					{
						free(csvp->table[i][j]->c_str);
						free(csvp->table[i][j]);
					}

					free(csvp->table[i]);
				}

				free(csvp->table);
				return NULL;
			}
			else
			{
				temp = StrAppend(buffer[rows - 1][currentColumn - 1], readChar);
				totalAllocation += (buffer[rows - 1][currentColumn - 1]->_length + 1) * sizeof(char);
				if (temp == NULL)
					return NULL;
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
				temp = (String **)realloc(buffer[rows - 1], (currentColumn + 1) * sizeof(String *));
				totalAllocation += (currentColumn + 1) * sizeof(String *);

				if (temp == NULL)
					return NULL;

				buffer[rows - 1] = temp;

				temp = StrCreateEmpty();
				totalAllocation += sizeof(String);
				if (temp == NULL)
					return NULL;

				buffer[rows - 1][currentColumn] = temp;
				currentColumn++;
				break;

			case '"':
				temp = StrAppend(buffer[rows - 1][currentColumn - 1], readChar);
				totalAllocation += (buffer[rows - 1][currentColumn - 1]->_length + 1) * sizeof(char);
				if (temp == NULL)
					return NULL;

				inQuote = true;
				break;

			case '\n':
				cells++;
				rows++;
				temp = (String ***)realloc(buffer, rows * sizeof(String **));
				totalAllocation += rows * sizeof(String **);
				if (temp == NULL)
					return NULL;

				buffer = temp;

				temp = (String **)calloc(1, sizeof(String *));
				totalAllocation += sizeof(String *);
				if (temp == NULL)
					return NULL;

				buffer[rows - 1] = temp;

				temp = StrCreateEmpty();
				totalAllocation += sizeof(char);
				if (temp == NULL)
					return NULL;

				buffer[rows - 1][0] = temp;

				if (rows == 2)
					cols = currentColumn;
				else if (currentColumn != cols)
					return NULL;

				currentColumn = 1;
				break;

			case EOF:
				if (currentColumn != cols && currentColumn != 1)
					return NULL;
				else if (lastChar == '\n')
				{
					for (unsigned short i = 0; i < currentColumn; i++)
					{
						free(buffer[rows - 1][i]->c_str);
						free(buffer[rows - 1][i]);
					}

					free(buffer[rows - 1]);
					rows--;
					cells--;
					currentColumn = cols;
				}
				break;

			default:
				temp = StrAppend(buffer[rows - 1][currentColumn - 1], readChar);
				totalAllocation += (buffer[rows - 1][currentColumn - 1]->_length + 1) * sizeof(char);

				if (temp == NULL)
					return NULL;

				break;
			}
		}
	} while (readChar != EOF);

	csvp->rows = rows;
	csvp->cols = cols;
	csvp->table = buffer;

	printf("Total allocated bytes: %lu\n", totalAllocation);
	return csvp;
}

String *CSVgetCell(CSV *csvp, unsigned long row, unsigned short col)
{
	if (row < csvp->rows && col < csvp->cols)
		return csvp->table[row][col];
	else
		return NULL;
}

char *CSVsetCell(CSV *csvp, unsigned long row, unsigned short col, char *str)
{
	if (row < csvp->rows && col < csvp->cols)
	{
		free(csvp->table[row * csvp->cols + col]);
		csvp->table[row * csvp->cols + col] = calloc(strlen(str) + 1, sizeof(char));
		strcpy((char *)(csvp->table[row * csvp->cols + col]), str);
	}
	return str;
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

void CSVprintRow(CSV *csvp, unsigned long row)
{
	char *colName = NULL;
	char *cellVal = NULL;
	for (unsigned short col = 0; col < csvp->cols; col++)
	{
		colName = CSVprocessString(*CSVgetCell(csvp, 0, col));
		cellVal = CSVprocessString(*CSVgetCell(csvp, row, col));
		printf("%s: %s\n", colName, cellVal);
		free(colName);
		free(cellVal);
	}
}

void CSVprintInfo(CSV *csvp)
{
	printf("Dokument CSV zawiera %lu wierszy po %hu kolumn kazdy.\n", csvp->rows - 1, csvp->cols);
}

void CSVclean(CSV *csvp)
{
	for (unsigned long i = 0; i < csvp->rows; i++)
	{
		for (unsigned short j = 0; j < csvp->cols; j++)
		{
			free(csvp->table[i][j]->c_str);
			free(csvp->table[i][j]);
		}

		free(csvp->table[i]);
	}

	free(csvp->table);
}