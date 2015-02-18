//
// The following include files must come first.

#ifndef COOL_TREE_HANDCODE_H
#define COOL_TREE_HANDCODE_H

#include <iostream>
#include "tree.h"
#include "cool.h"
#include "stringtab.h"
#define yylineno curr_lineno;
extern int yylineno;

inline Boolean copy_Boolean(Boolean b) {return b; }
inline void assert_Boolean(Boolean) {}
inline void dump_Boolean(ostream& stream, int padding, Boolean b)
	{ stream << pad(padding) << (int) b << "\n"; }

void dump_Symbol(ostream& stream, int padding, Symbol b);
void assert_Symbol(Symbol b);
Symbol copy_Symbol(Symbol b);

class Program_class;
typedef Program_class *Program;
class Class__class;
typedef Class__class *Class_;
class Feature_class;
typedef Feature_class *Feature;
class Formal_class;
typedef Formal_class *Formal;
class Expression_class;
typedef Expression_class *Expression;
class Case_class;
typedef Case_class *Case;

typedef list_node<Class_> Classes_class;
typedef Classes_class *Classes;
typedef list_node<Feature> Features_class;
typedef Features_class *Features;
typedef list_node<Formal> Formals_class;
typedef Formals_class *Formals;
typedef list_node<Expression> Expressions_class;
typedef Expressions_class *Expressions;
typedef list_node<Case> Cases_class;
typedef Cases_class *Cases;

#define Program_EXTRAS                          \
virtual void semant() = 0;			\
virtual void dump_with_types(ostream&, int) = 0; 



#define program_EXTRAS                          \
void semant();     				\
void dump_with_types(ostream&, int);            

#define Class__EXTRAS                   \
virtual Symbol get_filename() = 0;      \
virtual Symbol get_type() = 0;		\
virtual Symbol get_parent() = 0;    \
virtual Features get_features() = 0;	\
virtual void dump_with_types(ostream&,int) = 0; 


#define class__EXTRAS                                 \
Symbol get_filename() { return filename; }             \
Symbol get_type() { return name; }			\
Symbol get_parent() { return parent; }	\
Features get_features() { return features; }	\
void dump_with_types(ostream&,int);                    


#define Feature_EXTRAS                                        \
virtual Symbol get_name() = 0; \
virtual Symbol get_type() = 0; \
virtual int get_kind() = 0; \
virtual Formals get_formals() = 0;  \
virtual Expression get_expr() = 0;  \
virtual void dump_with_types(ostream&,int) = 0; 


#define Feature_SHARED_EXTRAS                                       \
void dump_with_types(ostream&,int);    

#define method_EXTRAS	\
Symbol get_name() { return name; }    \
Symbol get_type() { return return_type; }   \
Expression get_expr() { return expr; }  \
Formals get_formals() { return formals; } \
int get_kind() { return 0; }

#define attr_EXTRAS	\
Symbol get_name() { return name; }    \
Symbol get_type() { return type_decl; }	\
Expression get_expr() { return init; }  \
Formals get_formals() { return new nil_node<Formal>(); } \
int get_kind() { return 1; }

#define Formal_EXTRAS                              \
virtual Symbol get_name() = 0;	\
virtual Symbol get_type() = 0;	\
virtual void dump_with_types(ostream&,int) = 0;


#define formal_EXTRAS                           \
Symbol get_name() { return name; }  \
Symbol get_type() { return type_decl; }	\
void dump_with_types(ostream&,int);


#define Case_EXTRAS                             \
virtual Symbol get_name() = 0;	\
virtual Symbol  get_type() = 0;\
virtual Expression get_expr() = 0;\
virtual void dump_with_types(ostream& ,int) = 0;


#define branch_EXTRAS                                   \
void dump_with_types(ostream& ,int);	\
Symbol get_name() { return name; }\
Symbol  get_type() { return type_decl;}	\
Expression get_expr() { return expr; } \

#define Expression_EXTRAS                    \
Symbol type;                                 \
Symbol get_type() { return type; }           \
Expression set_type(Symbol s) { type = s; return this; } \
virtual Symbol get_type_name() { return NULL;}  \
virtual void dump_with_types(ostream&,int) = 0;  \
void dump_type(ostream&, int);               \
virtual Symbol get_name() { return NULL; }  \
virtual int get_kind() {return 0;} \
virtual Expression get_expr() { return NULL; }	\
Expression_class() { type = (Symbol) NULL; }	\
virtual Expressions get_exprs() { return NULL; }    \
virtual Expression get_pred() { return NULL; }  \
virtual Expression get_then_exp() { return NULL; }  \
virtual Expression get_else_exp() { return NULL; }  \
virtual Expression get_expr_init() { return NULL; } \
virtual Expression get_expr_pred() { return NULL; } \
virtual Expression get_expr_body() { return NULL; } \
virtual Expression get_e1() { return NULL; } \
virtual Expression get_e2() { return NULL; } \
virtual Cases get_cases() { return NULL; }  \


#define Expression_SHARED_EXTRAS           \
void dump_with_types(ostream&,int); 

#define  no_expr_EXTRAS	\
int get_kind() { return 0; }

#define int_const_EXTRAS    \
Symbol get_name() { return token; }  \
int get_kind() { return 1; }

#define bool_const_EXTRAS   \
Symbol get_name() { return (Symbol)val; }  \
int get_kind() { return 2; }	\

#define string_const_EXTRAS	\
Symbol get_name() { return token; }  \
int get_kind() { return 3; }	\

#define assign_EXTRAS	\
Symbol get_name() { return name; }  \
int get_kind() { return 4; }	\
Expression get_expr() { return expr; }

#define new__EXTRAS \
Symbol get_type_name() { return type_name; }	\
int get_kind() { return 5; }

#define block_EXTRAS \
int get_kind() { return 6; }	\
Expressions get_exprs() { return body; }

#define object_EXTRAS	\
int get_kind() { return 7; }	\
Symbol get_name() { return name; }

#define dispatch_EXTRAS	\
int get_kind() { return 8; }	\
Expressions get_exprs() { return actual; }  \
Symbol get_name() { return name; }  \
Expression get_expr() { return expr; }

#define static_dispatch_EXTRAS	\
int get_kind() { return 9; }	\
Expressions get_exprs() { return actual; }  \
Symbol get_name() { return name; }  \
Expression get_expr() { return expr; }	\
Symbol get_type_name() { return type_name; }

#define cond_EXTRAS \
int get_kind() { return 10; }	\
Expression get_pred() { return pred; }	\
Expression get_then_exp() { return then_exp; }	\
Expression get_else_exp() { return else_exp; }	\

#define let_EXTRAS \
int get_kind() { return 11; }	\
Symbol get_name() { return identifier; }    \
Symbol get_type_name() { return type_decl; }	\
Expression get_expr_init() { return init; }	\
Expression get_expr_body() { return body; }	

#define typcase_EXTRAS	\
int get_kind() { return 12; }	\
Expression get_expr() { return expr; }	\
Cases get_cases() { return cases; }

#define loop_EXTRAS \
int get_kind() { return 13; }	\
Expression get_expr_pred() { return pred; }\
Expression get_expr_body() { return body; }

#define isvoid_EXTRAS \
int get_kind() { return 14; }	\
Expression get_expr() { return e1; }

#define comp_EXTRAS \
int get_kind() { return 15; }	\
Expression get_expr() { return e1; }

#define lt_EXTRAS   \
int get_kind() { return 16; }	\
Expression get_e1() { return e1; }  \
Expression get_e2() { return e2; }  

#define leq_EXTRAS   \
int get_kind() { return 17; }	\
Expression get_e1() { return e1; }  \
Expression get_e2() { return e2; }  

#define neg_EXTRAS   \
int get_kind() { return 18; }	\
Expression get_e1() { return e1; }  \

#define plus_EXTRAS   \
int get_kind() { return 19; }	\
Expression get_e1() { return e1; }  \
Expression get_e2() { return e2; }  

#define sub_EXTRAS   \
int get_kind() { return 20; }	\
Expression get_e1() { return e1; }  \
Expression get_e2() { return e2; }  

#define mul_EXTRAS   \
int get_kind() { return 21; }	\
Expression get_e1() { return e1; }  \
Expression get_e2() { return e2; }  

#define divide_EXTRAS   \
int get_kind() { return 22; }	\
Expression get_e1() { return e1; }  \
Expression get_e2() { return e2; }  

#define eq_EXTRAS   \
int get_kind() { return 23; }	\
Expression get_e1() { return e1; }  \
Expression get_e2() { return e2; }  

#endif
