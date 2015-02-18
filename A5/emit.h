///////////////////////////////////////////////////////////////////////
//
//  Assembly Code Naming Conventions:
//
//     Dispatch table            <classname>_dispTab
//     Method entry point        <classname>.<method>
//     Class init code           <classname>_init
//     Abort method entry        <classname>.<method>.Abort
//     Prototype object          <classname>_protObj
//     Integer constant          int_const<Symbol>
//     String constant           str_const<Symbol>
//
///////////////////////////////////////////////////////////////////////

#include "stringtab.h"

#define MAXINT  100000000    
#define WORD_SIZE    4
#define LOG_WORD_SIZE 2     // for logical shifts

// Global names
#define CLASSNAMETAB         "class_nameTab"
#define CLASSOBJTAB          "class_objTab"
#define INTTAG               "_int_tag"
#define BOOLTAG              "_bool_tag"
#define STRINGTAG            "_string_tag"
#define HEAP_START           "heap_start"

// Naming conventions
#define DISPTAB_SUFFIX       "_dispTab"
#define METHOD_SEP           "."
#define CLASSINIT_SUFFIX     "_init"
#define PROTOBJ_SUFFIX       "_protObj"
#define OBJECTPROTOBJ        "Object"PROTOBJ_SUFFIX
#define INTCONST_PREFIX      "int_const"
#define STRCONST_PREFIX      "str_const"
#define BOOLCONST_PREFIX     "bool_const"


#define EMPTYSLOT            0
#define LABEL                ":\n"
#define VOID		     "0"
#define VOID_SIZE	     1

#define STRINGNAME (char *) "String"
#define INTNAME    (char *) "Int"
#define BOOLNAME   (char *) "Bool"
#define MAINNAME   (char *) "Main"

#define INT_CONST0		"int_const0"
#define INT_CONST0_LEN		10
#define STRING_CONST0		"str_const0"
#define STRING_CONST0_LEN	13
#define BOOL_CONST0		"0"
#define BOOL_CONST0_LEN		1

#define DISPATCH_ABORT		"_dispatch_abort"

//
// information about object headers
//
#define DEFAULT_OBJFIELDS 3
#define TAG_OFFSET 0
#define SIZE_OFFSET 1
#define DISPTABLE_OFFSET 2

#define INT_ATTR_OFFSET	    3
#define ATTR_OFFSET	    3
#define REGS_STORE_LEN	    3

#define STRING_SLOTS      1
#define INT_SLOTS         1
#define BOOL_SLOTS        1

#define GLOBAL        "\t.globl\t"
#define ALIGN         "\t.align\t2\n"
#define WORD          "\t.word\t"

//
// register names
//
#define ZERO "$zero"		// Zero register 
#define ACC  "$a0"		// Accumulator 
#define A1   "$a1"		// For arguments to prim funcs 
#define SELF "$s0"		// Ptr to self (callee saves) 
#define T1   "$t1"		// Temporary 1 
#define T2   "$t2"		// Temporary 2 
#define T3   "$t3"		// Temporary 3 
#define SP   "$sp"		// Stack pointer 
#define FP   "$fp"		// Frame pointer 
#define RA   "$ra"		// Return address 

//
// Opcodes
//
#define JALR  "\tjalr\t"  
#define JAL   "\tjal\t"                 
#define RET   "\tjr\t"RA"\t"

#define SW    "\tsw\t"
#define LW    "\tlw\t"
#define LI    "\tli\t"
#define LA    "\tla\t"

#define MOVE  "\tmove\t"
#define NEG   "\tneg\t"
#define ADD   "\tadd\t"
#define ADDI  "\taddi\t"
#define ADDU  "\taddu\t"
#define ADDIU "\taddiu\t"
#define DIV   "\tdiv\t"
#define MUL   "\tmul\t"
#define SUB   "\tsub\t"
#define SLL   "\tsll\t"
#define BEQZ  "\tbeqz\t"
#define BRANCH   "\tb\t"
#define BEQ      "\tbeq\t"
#define BNE      "\tbne\t"
#define BLEQ     "\tble\t"
#define BLT      "\tblt\t"
#define BGT      "\tbgt\t"

