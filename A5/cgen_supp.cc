#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "stringtab.h"

static int ascii = 0;

void ascii_mode(ostream& str)
{
  if (!ascii) 
    {
      str << "\t.ascii\t\"";
      ascii = 1;
    } 
}

void byte_mode(ostream& str)
{
  if (ascii) 
    {
      str << "\"\n";
      ascii = 0;
    }
}

void emit_string_constant(ostream& str, char* s)
{
  ascii = 0;

  while (*s) {
    switch (*s) {
    case '\n':
      ascii_mode(str);
      str << "\\n";
      break;
    case '\t':
      ascii_mode(str);
      str << "\\t";
      break;
    case '\\':
      byte_mode(str);
      str << "\t.byte\t" << (int) ((unsigned char) '\\') << endl;
      break;
    case '"' :
      ascii_mode(str);
      str << "\\\"";
      break;
    default:
      if (*s >= ' ' && ((unsigned char) *s) < 128) 
	{
	  ascii_mode(str);
	  str << *s;
	}
      else 
	{
	  byte_mode(str);
	  str << "\t.byte\t" << (int) ((unsigned char) *s) << endl;
	}
      break;
    }
    s++;
  }
  byte_mode(str);
  str << "\t.byte\t0\t" << endl;
}

Symbol concatSymbol(Symbol s1, Symbol s2) {
    int len1 = s1->get_len(), len2 = s2->get_len();
    char *str1 = s1->get_string(), *str2 = s2->get_string();
    int len = len1 + len2;
    char str[len];
    int i = 0;
    for (int j = 0; j < len1; j++) str[i++] = str1[j];
    for (int j = 0; j < len2; j++) str[i++] = str2[j];
    Symbol s = new StringEntry(str, len, 0);
    return s;
}
