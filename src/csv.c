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

		if (buffer == NULL)
		{
			buffer = (char **)calloc(1, sizeof(char *));
			(*buffer) = (char *)calloc(1, sizeof(char));
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
						for (unsigned long i = 0; i < cells; i++)
							free(buffer[i]);

						free(buffer);
						return NULL;
					}
					else
					{
						if (quotesInRow % 2 == 1)
						{
							temp = (char *)realloc(buffer[cells - 1], (positionInCell + quotesInRow - 1 + 1) * sizeof(char));
							if (temp == NULL)
								return NULL;

							buffer[cells - 1] = temp;
							for (unsigned short i = 0; i < quotesInRow - 1; positionInCell++, i++)
								buffer[cells - 1][positionInCell] = '"';

							inQuote = false;
						}
						else
						{
							temp = (char *)realloc(buffer[cells - 1], (positionInCell + quotesInRow + 1) * sizeof(char));
							if (temp == NULL)
								return NULL;

							buffer[cells - 1] = temp;
							for (unsigned short i = 0; i < quotesInRow; positionInCell++, i++)
								buffer[cells - 1][positionInCell] = '"';
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
				for (unsigned long i = 0; i < cells; i++)
					free(buffer[i]);

				free(buffer);
				return NULL;
			}
			else
			{
				temp = (char *)realloc(buffer[cells - 1], (positionInCell + 2) * sizeof(char));

				if (temp == NULL)
					return NULL;

				buffer[cells - 1] = temp;
				buffer[cells - 1][positionInCell] = readChar;
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
				buffer[cells - 1][positionInCell] = '\0';
				positionInCell = 0;
				cells++;
				temp = (char **)realloc(buffer, cells * sizeof(char *));

				if (temp == NULL)
					return NULL;

				buffer = temp;
				temp = (char *)calloc(1, sizeof(char));

				if (temp == NULL)
					return NULL;

				buffer[cells - 1] = temp;
				currentColumn++;
				break;

			case '"':
				inQuote = true;
				break;

			case '\n':
				buffer[cells - 1][positionInCell] = '\0';
				positionInCell = 0;
				cells++;
				rows++;
				temp = (char **)realloc(buffer, cells * sizeof(char *));

				if (temp == NULL)
					return NULL;

				buffer = temp;
				temp = (char *)calloc(1, sizeof(char));

				if (temp == NULL)
					return NULL;

				buffer[cells - 1] = temp;

				if (rows == 2)
					cols = currentColumn;
				else if (currentColumn != cols)
					return NULL;

				currentColumn = 1;
				break;

			case EOF:
				if (lastChar == '\n')
				{
					free(buffer[cells - 1]);
					temp = (char **)realloc(buffer, (cells - 1) * sizeof(char *));

					if (temp == NULL)
						return NULL;

					buffer = temp;
					cells--;
					rows--;
					positionInCell = strlen(buffer[cells - 1]);
				}
				break;

			default:
				temp = (char *)realloc(buffer[cells - 1], (positionInCell + 2) * sizeof(char));

				if (temp == NULL)
					return NULL;

				buffer[cells - 1] = temp;
				buffer[cells - 1][positionInCell] = readChar;
				positionInCell++;
				break;
			}
		}
	} while (readChar != EOF);

	buffer[cells - 1][positionInCell] = '\0';

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
	printf("Dokument CSV zawiera %lu wierszy po %hu kolumn kazdy.\n", csvp->rows - 1, csvp->cols);
}

void CSVclean(CSV *csvp)
{
	for (unsigned long i = 0; i < csvp->rows * csvp->cols; i++)
		free(csvp->table[i]);

	free(csvp->table);
}