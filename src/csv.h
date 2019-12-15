#pragma once

#include <stdio.h>

#include "Estring.h"
typedef struct
{
	unsigned short cols;
	unsigned long rows;
	String ***table;
} CSV;

enum CSVErrorCodes
{
	CSVNoError,
	CSVAllocationError,
	CSVIndexError,
	CSVEmptyFile,
	CSVFileIsCorrupted,
};

CSV *CSVreadFile(CSV *csvp, FILE *fp, short *error);
unsigned long CSVgetRows(CSV *csvp);
unsigned short CSVgetCols(CSV *csvp);
String *CSVgetCell(CSV *csvp, unsigned long row, unsigned short col, short *error);
char *CSVsetCell(CSV *csvp, unsigned long row, unsigned short col, char *str);
char *CSVprocessString(String str);
void CSVprintRow(CSV *csvp, unsigned long row, short *error);
void CSVprintInfo(CSV *csvp);
void CSVclean(CSV *csvp);
void CSVprintErrorMsg(short errorCode);
