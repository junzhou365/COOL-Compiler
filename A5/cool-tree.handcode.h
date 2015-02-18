//
// The following include files must come first.

#ifndef COOL_TREE_HANDCODE_H
#define COOL_TREE_HANDCODE_H

#include <iostream>
#include "tree.h"
#include "cool.h"
#include "stringtab.h"
#include "list.h"
#define yylineno curr_lineno;

enum feature_k {attr_k, method_k};
enum expr_k {has_expr_k, no_expr_k, int_const_k, string_const_k, bool_const_k};

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
virtual void cgen(ostream&) = 0;		\
virtual void dump_with_types(ostream&, int) = 0; 



#define program_EXTRAS                          \
void cgen(ostream&);     			\
void dump_with_types(ostream&, int);            

#define Class__EXTRAS                   \
virtual Symbol get_name() = 0;  	\
virtual Symbol get_parent() = 0;    	\
virtual Symbol get_filename() = 0;      \
virtual Features get_features() = 0;	\
virtual void dump_with_types(ostream&,int) = 0; 


#define class__EXTRAS                                  \
Symbol get_name()   { return name; }		       \
Symbol get_parent() { return parent; }     	       \
Symbol get_filename() { return filename; }             \
Features get_features() { return features; }	\
void dump_with_types(ostream&,int);                    


#define Feature_EXTRAS                                        \
virtual feature_k get_kind() = 0;  \
virtual Symbol get_name() = 0;  \
virtual Symbol get_type() = 0;	\
virtual Expression get_expr() = 0;   \
virtual Formals get_formals() { return NULL; }	\
virtual void dump_with_types(ostream&,int) = 0; 


#define Feature_SHARED_EXTRAS                                       \
void dump_with_types(ostream&,int);    

#define attr_EXTRAS \
feature_k get_kind() { return attr_k; }	\
Expression get_expr() { return init; }	\
Symbol get_type() { return type_decl; }	\
Symbol get_name() { return name; }

#define method_EXTRAS	\
feature_k get_kind() { return method_k; }	\
Formals get_formals() { return formals; }   \
Expression get_expr() { return expr; }	\
Symbol get_type() { return return_type; }   \
Symbol get_name() { return name; }


#define Formal_EXTRAS                              \
virtual Symbol get_name() = 0;	\
virtual void dump_with_types(ostream&,int) = 0;


#define formal_EXTRAS                           \
Symbol get_name() { return name; }  \
void dump_with_types(ostream&,int);


#define Case_EXTRAS                             \
virtual void dump_with_types(ostream& ,int) = 0;


#define branch_EXTRAS                                   \
void dump_with_types(ostream& ,int);


#define Expression_EXTRAS                    \
Symbol type;                                 \
Symbol get_type() { return type; }           \
Expression set_type(Symbol s) { type = s; return this; } \
virtual void code(ostream&, List<Entry>*, List<Entry>*, Feature) = 0; \
virtual expr_k get_kind() { return has_expr_k;}		\
virtual void dump_with_types(ostream&,int) = 0;  \
void dump_type(ostream&, int);               \
Expression_class() { type = (Symbol) NULL; }

#define Expression_SHARED_EXTRAS           \
void code(ostream&, List<Entry>*, List<Entry>*, Feature); 			   \
void dump_with_types(ostream&,int); 

#define no_expr_EXTRAS	\
expr_k get_kind() { return no_expr_k; }		

#define int_const_EXTRAS \
expr_k get_kind() { return int_const_k; }

#define string_const_EXTRAS \
expr_k get_kind() { return string_const_k; }

#define bool_const_EXTRAS \
expr_k get_kind() { return bool_const_k; }

#endif
