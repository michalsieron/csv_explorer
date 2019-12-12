#pragma once

typedef struct
{
	char *c_str;
	unsigned short _length;
} String;

String *StrCreateEmpty();
String *StrFromCStr(char *cstr);
String *StrAppend(String *str, char chr);
String *StrSetCStr(String *str, char *cstr);
