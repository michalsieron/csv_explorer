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
	char **buffer = NULL;
	char **header = NULL;
	char readChar = '\0';
	char lastChar = '\0';

	unsigned long rows = 1;
	unsigned short cols = 1;
	unsigned short currentColumn = 1;
	unsigned long cells = 1;
	unsigned short positionInCell = 0;

	bool inQuote = false;
	void *temp = NULL;

	do
	{
		readChar = fgetc(fp);
		// printf("i read char: %3hhd | %c\n", readChar, readChar);

		if (buffer == NULL)
		{
			buffer = (char **)calloc(1, sizeof(char *));
			(*buffer) = (char *)calloc(1, sizeof(char));
		}

		if (inQuote)
		{
			// printf("inQuote\n");
			temp = (char *)realloc(buffer[cells - 1], (positionInCell + 2) * sizeof(char));
			if (temp != NULL)
			{
				buffer[cells - 1] = temp;
			}
			else
			{
				// don't know what to do here yet
			}
			// printf("succesfully realloced space\n");
			// printf("just before condition lastChar = %hhd and readChar = %hhd\n", lastChar, readChar);
			if (lastChar == '"' && readChar != '"')
			{
				// printf("exiting Quote\n");
				inQuote = false;
			}
			buffer[cells - 1][positionInCell] = lastChar;
			// printf("writing %c to buffer[%lu][%hu] at %p\n", lastChar, cells - 1, positionInCell, buffer[cells - 1] + positionInCell);
			positionInCell++;
			lastChar = readChar;
		}
		else
		{
			switch (lastChar)
			{
			case '\0':
			case '\r':
				break;

			case ',':
				buffer[cells - 1][positionInCell] = '\0';
				// printf("writing \\0 to buffer[%lu][%hu] at %p\n", cells - 1, positionInCell, buffer[cells - 1] + positionInCell);
				positionInCell = 0;
				cells++;
				{
					temp = (char **)realloc(buffer, cells * sizeof(char *));
					if (temp != NULL)
					{
						buffer = temp;
						temp = (char *)calloc(1, sizeof(char));
						if (temp != NULL)
						{
							buffer[cells - 1] = temp;
						}
						else
						{
							// don't know what to do here yet
						}
					}
					else
					{
						// don't know what to do here yet
					}
				}
				currentColumn++;
				// printf("Last completed cell: %s\n", buffer[cells - 1]);
				break;

			case '"':
				// printf("in '\"' case\n");
				temp = (char *)realloc(buffer[cells - 1], (positionInCell + 2) * sizeof(char));
				if (temp != NULL)
				{
					buffer[cells - 1] = temp;
				}
				else
				{
					// don't know what to do here yet
				}
				// printf("succesfully realloced space\n");
				buffer[cells - 1][positionInCell] = lastChar;
				// printf("writing %c to buffer[%lu][%hu] at %p\n", lastChar, cells - 1, positionInCell, buffer[cells - 1] + positionInCell);
				positionInCell++;

				// if (readChar != '"')
				inQuote = true;
				// printf("set inQuote to true\n");
				// printf("exiting '\"' case\n");
				break;

			case '\n':
				buffer[cells - 1][positionInCell] = '\0';
				// printf("writing \\0 to buffer[%lu][%hu] at %p\n", cells - 1, positionInCell, buffer[cells - 1] + positionInCell);
				positionInCell = 0;
				cells++;
				rows++;
				temp = (char **)realloc(buffer, cells * sizeof(char *));
				if (temp != NULL)
				{
					buffer = temp;
					temp = (char *)calloc(1, sizeof(char));
					if (temp != NULL)
					{
						buffer[cells - 1] = temp;
					}
					else
					{
						// don't know what to do here yet
					}
				}
				else
				{
					// don't know what to do here yet
				}
				if (rows == 2)
				{
					cols = currentColumn;
				}
				else if (currentColumn != cols)
				{
					return NULL;
				}

				if (readChar == EOF)
				{
					free(buffer[cells - 1]);
					temp = (char **)realloc(buffer, (cells - 1) * sizeof(char *));
					if (temp != NULL)
					{
						buffer = temp;
					}
					else
					{
						// don't know what to do here yet
					}
					cells--;
					rows--;
					positionInCell = strlen(buffer[cells - 1]);
				}

				currentColumn = 1;
				// printf("Last completed cell: %s\n", buffer[cells - 1]);
				break;

			default:
				temp = (char *)realloc(buffer[cells - 1], (positionInCell + 2) * sizeof(char));
				if (temp != NULL)
				{
					buffer[cells - 1] = temp;
				}
				else
				{
					// don't know what to do here yet
				}
				buffer[cells - 1][positionInCell] = lastChar;
				// printf("writing %c to buffer[%lu][%hu] at %p\n", lastChar, cells - 1, positionInCell, buffer[cells - 1] + positionInCell);
				positionInCell++;
				break;
			}
			lastChar = readChar;
		}
	} while (readChar != EOF);

	buffer[cells - 1][positionInCell] = '\0';
	printf("writing \\0 to buffer[%lu][%hu] at %p\n", cells - 1, positionInCell, buffer[cells - 1] + positionInCell);

	printf("Liczba komorek: %lu\n", cells);
	csvp->rows = rows;
	csvp->cols = cols;
	csvp->table = buffer;

	return csvp;
}

char *CSVgetCell(CSV *csvp, unsigned long row, unsigned short col)
{
	return csvp->table[row * csvp->cols + col];
}

char *CSVsetCell(CSV *csvp, unsigned long row, unsigned short col, char *str)
{
	free(csvp->table[row * csvp->cols + col]);
	csvp->table[row * csvp->cols + col] = calloc(strlen(str) + 1, sizeof(char));
	strcpy(csvp->table[row * csvp->cols + col], str);
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
	printf("Dokument CSV zawiera %lu wierszy po %hu kolumn kazdy.\n", csvp->rows, csvp->cols);
}

void CSVclean(CSV *csvp)
{
	for (unsigned long i = 0; i < csvp->rows * csvp->cols; i++)
		free(csvp->table[i]);

	free(csvp->table);
}