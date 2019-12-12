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
	unsigned short positionInCell = 0;
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

						for (unsigned short i = 0; i < quotesInRow; positionInCell++, i++)
						{
							temp = StrAppend(buffer[rows - 1][currentColumn - 1], '"');
							totalAllocation += sizeof(char);
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
				totalAllocation += sizeof(char);
				if (temp == NULL)
					return NULL;
				positionInCell++;
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
				positionInCell = 0;
				cells++;
				temp = (String **)realloc(buffer[rows - 1], (currentColumn + 1) * sizeof(String *));
				totalAllocation += sizeof(String *);

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
				totalAllocation += sizeof(char);
				if (temp == NULL)
					return NULL;

				positionInCell++;
				inQuote = true;
				break;

			case '\n':
				positionInCell = 0;
				cells++;
				rows++;
				temp = (String ***)realloc(buffer, rows * sizeof(String **));
				totalAllocation += sizeof(String **);
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
					positionInCell = buffer[rows - 1][currentColumn - 1]->_length - 1;
				}
				break;

			default:
				temp = StrAppend(buffer[rows - 1][currentColumn - 1], readChar);
				totalAllocation += sizeof(char);

				if (temp == NULL)
					return NULL;

				positionInCell++;
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

char *CSVgetCell(CSV *csvp, unsigned long row, unsigned short col)
{
	return csvp->table[row][col]->c_str;
}

char *CSVsetCell(CSV *csvp, unsigned long row, unsigned short col, char *str)
{
	free(csvp->table[row * csvp->cols + col]);
	csvp->table[row * csvp->cols + col] = calloc(strlen(str) + 1, sizeof(char));
	strcpy((char *)(csvp->table[row * csvp->cols + col]), str);
	return str;
}

void CSVprintRow(CSV *csvp, unsigned long row)
{
	for (unsigned short col = 0; col < csvp->cols; col++)
	{
		printf("%s: %s\n", CSVgetCell(csvp, 0, col), CSVgetCell(csvp, row, col));
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