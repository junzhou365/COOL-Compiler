/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */
int nested_comments = 0;
int string_len = 0;
%}
%x comment dashed_comment
%x str error_str
/*
 * Define names for regular expressions here.
 */
DARROW          =>
ASSIGN		<-
/* My definition */
DIGIT	[0-9]
TYPEID	[A-Z][a-zA-Z0-9_]*
OBJECTID [a-z][a-zA-Z0-9_]*
%%
\.  return '.';
@   return '@';
~   return '~';
\*  return '*';
\/   return '/';
\+  return '+';
-   return '-';
\<   return '<';
=   return '=';
;   return ';';
:   return ':';
\{  return '{';
\}  return '}';
\(  return '(';
\)  return ')';
, return ',';
! {
    cool_yylval.error_msg = "!";
    return ERROR;
    }
# {
    cool_yylval.error_msg = "#";
    return ERROR;
    }
\$ {
    cool_yylval.error_msg = "$";
    return ERROR;
    }
% {
    cool_yylval.error_msg = "%";
    return ERROR;
    }
\^ {
    cool_yylval.error_msg = "^";
    return ERROR;
    }
& {
    cool_yylval.error_msg = "&";
    return ERROR;
    }
_ {
    cool_yylval.error_msg = "_";
    return ERROR;
    }
> {
    cool_yylval.error_msg = ">";
    return ERROR;
    }
\? {
    cool_yylval.error_msg = "?";
    return ERROR;
    }
` {
    cool_yylval.error_msg = "`";
    return ERROR;
    }
\[ {
    cool_yylval.error_msg = "[";
    return ERROR;
    }
\] {
    cool_yylval.error_msg = "]";
    return ERROR;
    }
\| {
    cool_yylval.error_msg = "|";
    return ERROR;
    }
\\ {
    cool_yylval.error_msg = "\\";
    return ERROR;
    }
[\001-\006] {
    cool_yylval.error_msg = yytext;
    return ERROR;
    }
 /*
  *  Nested comments
  */
\(\* BEGIN(comment);nested_comments = 1;
<comment><<EOF>> {
    cool_yylval.error_msg = "EOF in comment";
    BEGIN(INITIAL);
    return ERROR;
    }
<comment>\(\* nested_comments++;
<comment>\([^*]*
<comment>[^*(\n]*
<comment>\*+[^*)\n]*
<comment>\n curr_lineno++;
<comment>\*+\)	{
    if (--nested_comments == 0) BEGIN(INITIAL);
}
\*+\) {
    cool_yylval.error_msg = "Unmatched *)";
    return ERROR;
    }
-- BEGIN(dashed_comment);
<dashed_comment>[^\n]
<dashed_comment>\n  curr_lineno++;BEGIN(INITIAL);
<dashed_comment><<EOF>> yyterminate();
 /*
  *  The multiple-character operators.
  */
{DARROW}		{ return (DARROW); }
\<=			{ return (LE); }
{ASSIGN}		{ return (ASSIGN); }
[ \t]+
 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
  */
(?i:class) {return (CLASS);}
(?i:else) {return (ELSE);}
f(?i:alse) {
    cool_yylval.boolean = false;
    return BOOL_CONST;
    }
(?i:fi) {return (FI);}
(?i:if) {return (IF);}
(?i:in) {return (IN);}
(?i:inherits) {return (INHERITS);}
(?i:isvoid) {return (ISVOID);}
(?i:let) {return (LET);}
(?i:loop) {return (LOOP);}
(?i:pool) {return (POOL);}
(?i:then) {return (THEN);}
(?i:while) {return (WHILE);}
(?i:case) {return (CASE);}
(?i:esac) {return (ESAC);}
(?i:new) {return (NEW);}
(?i:of) {return (OF);}
(?i:not) {return (NOT);}
t(?i:rue) {
    cool_yylval.boolean = true;
    return BOOL_CONST;
    }
{DIGIT}+ {
    cool_yylval.symbol = inttable.add_string(yytext);
    return INT_CONST;
    }
{TYPEID}|self  {
    cool_yylval.symbol = idtable.add_string(yytext);
    return TYPEID;
    }
{OBJECTID} {
    cool_yylval.symbol = idtable.add_string(yytext);
    return OBJECTID;
    }
 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */
\" string_buf_ptr = string_buf;string_len = 0;BEGIN(str);
<str>\" {
    BEGIN(INITIAL);
    if (string_len < MAX_STR_CONST) {
	string_buf_ptr = '\0';
	cool_yylval.symbol = stringtable.add_string(string_buf, string_len);
	return STR_CONST;
    } else {
	cool_yylval.error_msg = "String constant too long";
	return ERROR;
    }
}
<error_str>[^\"\n]
<error_str>[\"\n] {
    BEGIN(INITIAL);
    cool_yylval.error_msg = "String contains null character.";
    return ERROR;
    }
<str>\n {
    cool_yylval.error_msg = "Unterminated string constant.";
    BEGIN(INITIAL);
    curr_lineno++;
    return ERROR;
    }
<str>  {
    BEGIN(error_str);
    }
<str><<EOF>> {
    cool_yylval.error_msg = "EOF in string constant";
    BEGIN(INITIAL);
    return ERROR;
    }
<str>\\b if (string_len < MAX_STR_CONST - 1) *string_buf_ptr++ = '\b';string_len++;
<str>\\t if (string_len < MAX_STR_CONST - 1) *string_buf_ptr++ = '\t';string_len++;
<str>\\n if (string_len < MAX_STR_CONST - 1) *string_buf_ptr++ = '\n';string_len++;
<str>\\f if (string_len < MAX_STR_CONST - 1) *string_buf_ptr++ = '\f';string_len++;

<str>\\(.|\n) {
    if (string_len < MAX_STR_CONST - 1) *string_buf_ptr++ = yytext[1];
    string_len++;
    if (yytext[1] == '\n') curr_lineno++;
    }
<str>[^\\\n\"\ ]+ {
    char *yptr = yytext;
    while (*yptr) {
	if (string_len < MAX_STR_CONST - 1)*string_buf_ptr++ = *yptr;
	string_len++;yptr++;
	}
    }
\n {curr_lineno++;}
<INITIAL><<EOF>> yyterminate();
%%

