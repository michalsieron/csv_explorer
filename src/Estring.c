#include <stdlib.h>
#include <string.h>

#include "Estring.h"

String *StrCreateEmpty()
{
	String *str;
	void *temp = (String *)calloc(1, sizeof(String));
	if (temp == NULL)
		return NULL;

	str = temp;
	str->_length = 1;

	temp = (char *)calloc(1, sizeof(char));
	if (temp == NULL)
	{
		free(str);
		return NULL;
	}

	str->c_str = temp;
	str->c_str[str->_length - 1] = 0;
	return str;
}

String *StrFromCStr(char *cstr)
{
	String *str;
	void *temp = (String *)calloc(1, sizeof(String));
	if (temp == NULL)
		return NULL;

	str = temp;
	str->_length = strlen(cstr) + 1;
	temp = (char *)calloc(str->_length, sizeof(char));
	if (temp == NULL)
	{
		free(str);
		return NULL;
	}

	str->c_str = temp;
	strcpy(str->c_str, cstr);
	str->c_str[str->_length - 1] = 0;
	return str;
}

String *StrAppend(String *str, char chr)
{
	char *temp = (char *)realloc(str->c_str, str->_length + 1);
	if (temp == NULL)
		return NULL;

	str->c_str = temp;
	str->_length++;
	str->c_str[str->_length - 2] = chr;
	str->c_str[str->_length - 1] = 0;
	return str;
}

String *StrSetCStr(String *str, char *cstr)
{
	str->_length = strlen(cstr) + 1;
	char *temp = (char *)realloc(str->c_str, str->_length * sizeof(char));
	if (temp == NULL)
		return NULL;

	str->c_str = temp;
	strcpy(str->c_str, cstr);
	str->c_str[str->_length - 1] = 0;
	return str;
}