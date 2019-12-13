#pragma once

#include <stdio.h>

#include "Estring.h"
typedef struct
{
	unsigned short cols;
	unsigned long rows;
	String ***table;
} CSV;

CSV *CSVreadFile(CSV *csvp, FILE *fp);
String *CSVgetCell(CSV *csvp, unsigned long row, unsigned short col);
char *CSVsetCell(CSV *csvp, unsigned long row, unsigned short col, char *str);
char *CSVprocessString(String str);
void CSVprintRow(CSV *csvp, unsigned long row);
void CSVprintInfo(CSV *csvp);
void CSVclean(CSV *csvp);
