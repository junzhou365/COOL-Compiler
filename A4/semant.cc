

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"


extern int semant_debug;
extern char *curr_filename;
Symbol check_types(SymbolTable<Symbol, Symbol>*, TypesMatrix*, ClassTable*,  Class_, Expression);
void update_method_n_attr(Class_ c, SymbolTable<Symbol, Symbol> *ObjEnv, TypesMatrix *MetEnv);
int semantic_trav_error_counter;
Symbol appendString(Symbol a, Symbol b);
bool compareSelf(Symbol);
Symbol partSelfString(Symbol a); 
void ds() { cout << "hhh" << endl;}
void abort_all() {
    cerr << "Compilation halted due to static semantic errors." << endl;
    exit(1);
}

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}



ClassTable::ClassTable(Classes classes) : semant_errors(0) , error_stream(cerr) {

    /* Fill this in */
    /* initialize the hash table */
    inGraph = new HashTable();
    inGraph->length = 0, inGraph->size = 10, inGraph->array = new Elem*[inGraph->size]();
    nDefClasses = new HashTable();
    nDefClasses->length = 0, nDefClasses->size = 10, nDefClasses->array = new Elem*[nDefClasses->size]();
    install_basic_classes();

    /* First pass, check inheritance */
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
	Class_ c = classes->nth(i);
	if (c->get_type() == SELF_TYPE){
	    semant_error(c) << "Wrong Type" << endl;
	    exit(1);
	} else {
	    if (search(inGraph, c->get_type())) {
		semant_error(c) << "Class is redefined or has more than one parent.\n";
		cerr << "Compilation halted due to static semantic errors." << endl;
		exit(1);
	    }
	    del(nDefClasses, c->get_type());
	    if (c->get_parent() == Bool || c->get_parent() == Str || c->get_parent() == Int || c->get_parent() == SELF_TYPE) {
		semant_error(c) << "Class " << c->get_type() << " cannot inherit from " << "Class " << c->get_parent() << endl;
		cerr << "Compilation halted due to static semantic errors." << endl;
		exit(1);
	    }    
	    Elem *elem_parent = search(inGraph, c->get_parent());
	    if (elem_parent) {
		
		insert(inGraph, c->get_type(), c, elem_parent);
	    } else {
		insert(inGraph, c->get_type(), c, NULL);
		insert(nDefClasses, c->get_parent(), c, NULL);
	    }
	}
    }
    if (nDefClasses->length) {
	for (int i = 0; i < nDefClasses->size; i++) {
	    if (nDefClasses->array[i]) {
		Symbol t = nDefClasses->array[i]->key;
		semant_error(nDefClasses->array[i]->vertex) << "Class " << t << " is not defined.\n";
	    }
	}
	delete nDefClasses->array;
	cerr << "Compilation halted due to static semantic errors." << endl;
	exit(1);
    }
    delete nDefClasses->array;
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
	Class_ c = classes->nth(i);
	Elem *elem_parent = search(inGraph, c->get_parent());
	if (elem_parent) {
	    search(inGraph, c->get_type())->parent = elem_parent;
	} else {
	    semant_error() << "Some Classes are not defined.\n";
	    exit(1);
	}
    }	
    for (int i = 0; i < inGraph->size; i++) {
	if (inGraph->array[i]) {
	    Elem *node = inGraph->array[i];
	    Elem *fast_node = node->parent;
	    while (fast_node && fast_node->key != Object) {
		if (node->key == fast_node->key) {
		    semant_error() << "Inheritance Loop\n";
		    exit(1);
		}
		node = node->parent;
		if (fast_node->parent) {
		    fast_node = fast_node->parent->parent;
		} else {
		    break;
		}
	    }
	}
    }
    

}

void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
   // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");
    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
	class_(Object, 
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = 
	class_(IO, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename);  

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
	class_(Int, 
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
	class_(Str, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat, 
								      single_Formals(formal(arg, Str)),
								      Str, 
								      no_expr()))),
			       single_Features(method(substr, 
						      append_Formals(single_Formals(formal(arg, Int)), 
								     single_Formals(formal(arg2, Int))),
						      Str, 
						      no_expr()))),
	       filename);
    
    /* Add Five basic classes to HashTable */
    insert(inGraph, Object, Object_class, NULL);
    insert(inGraph, IO, IO_class, search(inGraph, Object));
    insert(inGraph, Int, Int_class, search(inGraph, Object));
    insert(inGraph, Bool, Bool_class, search(inGraph, Object));
    insert(inGraph, Str, Str_class, search(inGraph, Object));
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
    return semant_error(c->get_filename(),c);
}    

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error()                  
{                                                 
    semant_errors++;                            
    return error_stream;
} 



/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{
    initialize_constants();
    semantic_trav_error_counter = 0;
    /* ClassTable constructor may do some semantic analysis */
    ClassTable *classtable = new ClassTable(classes);
    /* some semantic analysis code may go here */
    Classes basic_cls = nil_Classes();
    basic_cls = append_Classes(basic_cls, single_Classes(classtable->find(Object)->vertex));
    basic_cls = append_Classes(basic_cls, single_Classes(classtable->find(IO)->vertex));
    basic_cls = append_Classes(basic_cls, single_Classes(classtable->find(Int)->vertex));
    basic_cls = append_Classes(basic_cls, single_Classes(classtable->find(Str)->vertex));
    basic_cls = append_Classes(basic_cls, single_Classes(classtable->find(Bool)->vertex));
    
    if (!classtable->find(Main)) {
	cerr << "No main class is defined.\n";
	cerr << "Compilation halted due to static semantic errors." << endl;
	exit(1);
    }
    int classes_size = classes->len();
    SymbolTable<Symbol, Symbol> **ObjEnvs = new SymbolTable<Symbol, Symbol>*[classes_size + 5]();
    TypesMatrix *MetEnv = new TypesMatrix();
    ostream& error_stream(cerr);

    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
	Class_ c = classes->nth(i);
	/* enter a new scope; every class is a new scope */
	SymbolTable<Symbol, Symbol> *ObjEnv = new SymbolTable<Symbol, Symbol>();
	ObjEnvs[i] = ObjEnv;
	ObjEnv->enterscope();
	update_method_n_attr(c, ObjEnv, MetEnv);
    }
    for (int i = basic_cls->first(); basic_cls->more(i); i = basic_cls->next(i)) {
	Class_ c = basic_cls->nth(i);
	/* enter a new scope; every class is a new scope */
	SymbolTable<Symbol, Symbol> *ObjEnv = new SymbolTable<Symbol, Symbol>();
	ObjEnvs[i+classes_size] = ObjEnv;
	ObjEnv->enterscope();
	update_method_n_attr(c, ObjEnv, MetEnv);
    }
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
	Class_ c = classes->nth(i);
	/* enter a new scope; every class is a new scope */
	SymbolTable<Symbol, Symbol> *ObjEnv; 
	ObjEnv = ObjEnvs[i];
	for (int j = c->get_features()->first(); c->get_features()->more(j); j = c->get_features()->next(j)) {
	    Feature f = c->get_features()->nth(j);
	    /* attr */ 
	    /* returning 0 means no_expr(), which doesn't need initialization */
	    if (f->get_kind()) {
		if (f->get_expr()->get_kind()) {
		    ObjEnv->enterscope();
		    ObjEnv->addid(self, new Symbol(appendString(SELF_TYPE, c->get_type())));
		    Symbol temp_type = check_types(ObjEnv, MetEnv, classtable, c, f->get_expr());
		    if (compareSelf(temp_type))
			temp_type = partSelfString(temp_type);
		    ObjEnv->exitscope();
		    if (!classtable->check_conformance(temp_type, f->get_type()))
			error_stream << "Line#" << f->get_line_number() << ": Attribute " << f->get_name() << "'s type " << f->get_type() << " is mismatched with initial expression's type " << endl;
		}
	    }
	    else {
		/* mmethod */
		Formals fls = f->get_formals();
		Symbol t0 = f->get_type();
		/* new scope, store all ID : Types */
		ObjEnv->enterscope();
		for (int k = fls->first(); fls->more(k); k = fls->next(k)) {
		    Formal fl = fls->nth(k);
		    if (fl->get_type() == SELF_TYPE) {
			cerr << c->get_filename() << ":" << fl->get_line_number() << ": Foraml parameter " << fl->get_name() << " can't have the type "<< fl->get_type() << "\n";
			semantic_trav_error_counter++;
			abort_all();
		    }
		    ObjEnv->addid(fl->get_name(), new Symbol(fl->get_type()));
		}
		ObjEnv->addid(self, new Symbol(appendString(SELF_TYPE, c->get_type())));
		Symbol t0_pr = check_types(ObjEnv, MetEnv, classtable, c, f->get_expr());
		ObjEnv->exitscope();
		if (f->get_type() == SELF_TYPE) 
		    t0 = c->get_type();
		if (!classtable->check_conformance(t0_pr, t0)) {
		    cerr << c->get_filename()<<":"<<f->get_line_number() << ": " << f->get_type() << " is not defined or not conformed to\n";
		    semantic_trav_error_counter++;
		}
	    }
	}		
    }

    if (classtable->errors() || semantic_trav_error_counter) {
	cerr << "Compilation halted due to static semantic errors." << endl;
	exit(1);
    }
}
Symbol appendString(Symbol a, Symbol b) {
    char *str1 = a->get_string();
    char *str2 = b->get_string();
    int len1 = a->get_len(), len2 = b->get_len();
    char *str = new char[len1+len2+1];
    int index = 0;
    for (int i = 0; i < len1; i++)
	str[index++] = str1[i];
    str[index++] = '@';
    for (int i = 0; i < len2; i++)
	str[index++] = str2[i];
    Symbol output = new Entry(str, index, 0);
    return output;
}
bool compareSelf(Symbol a) {
    if (a->get_len() < 9) return false;
    char *str = "SELF_TYPE";
    char *str2 = a->get_string();
    int i = 0;
    while (i < 9) {
	if (str[i] != str2[i]) return false;
	i++;
    }
    return true;
}
Symbol partSelfString(Symbol a) {
    if (!compareSelf(a)) return NULL;
    char *str = a->get_string(); 
    int len = a->get_len();
    char *ostr = new char[len-10];
    for (int i = 10; i < len; i++)
	ostr[i-10] = str[i];
    Symbol output = new Entry(ostr, len-10, 0);
    return output;
}
Symbol partFormer(Symbol a) {
    int len = 0;
    char *str = a->get_string();
    while (str[len++] != '@' && len <= a->get_len()) {
    }
    char str2[len-1];
    for (int i = 0; i < len-1; i++)
	str2[i] = str[i];
    return new Entry(str2, len-1, 0);
}
Symbol partLater(Symbol a) {
    int len = 0;
    char *str = a->get_string();
    while (str[len++] != '@'&& len <= a->get_len())
	{}
    char str2[a->get_len()-len];
    for (int i = len; i < a->get_len(); i++) {
	str2[i-len] = str[i];
    }
    return new Entry(str2, a->get_len()-len, 0);
}
bool compare2(Symbol a, Symbol b) {
    if (a->get_len() != b->get_len())
	return false;
    char *s1 = a->get_string(), *s2 = b->get_string();
    for (int i = 0; i < a->get_len(); i++) {
	if (s1[i] != s2[i]) return false;
    }
    return true;
}
void update_method_n_attr(Class_ c, SymbolTable<Symbol, Symbol> *ObjEnv, TypesMatrix *MetEnv) 
{ 
    for (int j = c->get_features()->first(); c->get_features()->more(j); j = c->get_features()->next(j)) {
	Feature f = c->get_features()->nth(j);
	/* attr */ 
	if (f->get_kind()) {
	    if (self == f->get_name()) {
		cerr << c->get_filename() << ":" << f->get_line_number() << ":" << "attributes cannot be named self\n";
		semantic_trav_error_counter++;
		cerr << "Compilation halted due to static semantic errors." << endl;
		exit(1);
	    }
	    if (f->get_type() == SELF_TYPE)
		ObjEnv->addid(f->get_name(), new Symbol(appendString(SELF_TYPE, c->get_type())));
	    else
		ObjEnv->addid(f->get_name(), new Symbol (f->get_type()));
	}
	else {
	    /* mmethod */
	    Formals fls = f->get_formals();
	    /* update method environment */
	    Symbol t0 = f->get_type();
	    int formals_size = fls->len() + 1;
	    Symbol *types_array = new Symbol[formals_size];
	    /* new scope, store all ID : Types */
	    for (int k = fls->first(); fls->more(k); k = fls->next(k)) {
		Formal fl = fls->nth(k);
		if (self == fl->get_name()) {
		    cerr << c->get_filename() << ":" << fl->get_line_number() << ":" << "formal parameters cannot be named self\n";
		    semantic_trav_error_counter++;
		    cerr << "Compilation halted due to static semantic errors." << endl;
		    exit(1);
		}
		types_array[k] = fl->get_type();
	    }
	    /* add return type T0 */
	    types_array[formals_size-1] = t0;
	    /* feature might be redefined */
	    Symbol temp = appendString(f->get_name(), c->get_type());
	    MetEnv->push_back(types_array, temp, formals_size);
	}
    }		
}
Symbol check_types(SymbolTable<Symbol, Symbol> *ObjEnv, TypesMatrix *MetEnv, ClassTable *ct, Class_ c, Expression e) {
    Symbol t_pr, t, t0, t0_pr, t1;
    Expressions es;
    Symbol *array_pr;
    ostream& os(cerr);
    /* int */
    if (e->get_kind() == 1) {
	    e->set_type(Int); 
	    return Int;
    } /* bool */
    else if (e->get_kind() == 2) {
	    e->set_type(Bool);
	    return Bool;
    } /* string */
    else if (e->get_kind() == 3) {
	    e->set_type(Str);
	    return Str;
    } /* assign */
    else if (e->get_kind() == 4) {
	if (e->get_name() == self) {
	    cerr << c->get_filename() << ":" << e->get_line_number() << ":"<< " Cann't be assigned self" << endl;
	    semantic_trav_error_counter++;
	    cerr << "Compilation halted due to static semantic errors." << endl;
	    exit(1);
	}
	t = *ObjEnv->lookup(e->get_name());
	t_pr = check_types(ObjEnv, MetEnv, ct, c, e->get_expr());
	if (ct->check_conformance(t_pr, t)) {
	    e->set_type(idtable.add_string(t_pr->get_string()));
	    return t_pr;
	} else {
	    cerr << c->get_filename() << ":" << e->get_line_number() << ": " << e->get_name() << " Cann't be assigned due to conformation fault" << endl;
	    semantic_trav_error_counter++;
	    e->set_type(Object);
	    return Object;
	}
    } /* nnew */
    else if (e->get_kind() == 5) {
	    if (!ct->find(e->get_type_name()) && e->get_type_name() != SELF_TYPE) {
		cerr << c->get_filename()<<":"<<e->get_line_number() << ":"<<e->get_type_name()<<" is not defined\n";
		semantic_trav_error_counter++;
		e->set_type(Object);
		return Object;
	    }
	    if (e->get_type_name() == SELF_TYPE) {
		t_pr = appendString(SELF_TYPE, c->get_type());
		e->set_type(SELF_TYPE);
	    } else {
		t_pr = e->get_type_name();
		if (compareSelf(t_pr))
		    e->set_type(SELF_TYPE);
		else
		    e->set_type(idtable.add_string(t_pr->get_string()));
	    }
	    return t_pr;
    } /* block */
    else if (e->get_kind() == 6) {
	    es = e->get_exprs();
	    for (int i = es->first(); es->more(i); i = es->next(i)) {
		Expression en = es->nth(i);
		t = check_types(ObjEnv, MetEnv, ct, c, en);
	    }
	    if (compareSelf(t))
		e->set_type(SELF_TYPE);
	    else
		e->set_type(idtable.add_string(t->get_string()));
	    return t;
    } /* object id */
    else if (e->get_kind() == 7) {
	Symbol *s_pr = ObjEnv->lookup(e->get_name());
	if (!s_pr) { 
	    cerr << c->get_filename() << ":" << e->get_line_number() << ": Undeclared type " << e->get_name() << endl;
	    semantic_trav_error_counter++;
	    return Object;
	}
	t = *s_pr;
	if (compareSelf(t))
	    e->set_type(SELF_TYPE);
	else
	    e->set_type(idtable.add_string(t->get_string()));
	return t;
    } /* dispatch */
    else if (e->get_kind() == 8) {
	if (e->get_expr()->get_kind()) {
	    /* e0.<id>(<e1>,...,<en>) */
	    t0 = check_types(ObjEnv, MetEnv, ct, c, e->get_expr());
	    if (compareSelf(t0))
		t0_pr = partSelfString(t0);
	    else
		t0_pr = t0;
	}	
	es = e->get_exprs();
	array_pr = MetEnv->find(appendString(e->get_name(), t0_pr), ct);
	if (!array_pr) {
	    cerr << c->get_filename() << ":" << e->get_line_number() << ":" << e->get_name() << " is not found" << endl;
	    semantic_trav_error_counter++;
	    e->set_type(Object);
	    return Object;
	}
	int size = 0;
	for (int i = es->first(); es->more(i); i = es->next(i)) {
	    Expression en = es->nth(i);
	    t = check_types(ObjEnv, MetEnv, ct, c, en);
	    if (!ct->check_conformance(t, array_pr[i])) {
		cerr <<c->get_filename()<<":"<<en->get_line_number()<<":"<<en->get_name()<<" type mismatch"<<endl;
		semantic_trav_error_counter++;
	    }
	    size++;
	}
	t_pr = array_pr[size];
	if (t_pr == SELF_TYPE) {
	    if (compareSelf(t_pr))
		e->set_type(SELF_TYPE);
	    else
		e->set_type(idtable.add_string(t0->get_string()));
	    return t0;
	} else {
	    if (compareSelf(t_pr))
		e->set_type(SELF_TYPE);
	    else
		e->set_type(idtable.add_string(t_pr->get_string()));
	    return t_pr;
	}
    } /* static dispatch */
    else if (e->get_kind() == 9) {
	    t0 = check_types(ObjEnv, MetEnv, ct, c, e->get_expr());
	    t = e->get_type_name();
	    es = e->get_exprs();
	    Symbol temp = appendString(e->get_name(), t);
	    array_pr = MetEnv->find(temp, ct);
	    if (!array_pr) {
		cerr << c->get_filename() << ":" << e->get_line_number() << ":" << e->get_type_name() << " is not found" << endl;
		semantic_trav_error_counter++;
		e->set_type(Object);
		return Object;
	    }    
	    int size = 0;
	    for (int i = es->first(); es->more(i); i = es->next(i)) {
		Expression en = es->nth(i);
		t = check_types(ObjEnv, MetEnv, ct, c, en);
		en->set_type(idtable.add_string(t->get_string()));
		if (!ct->check_conformance(t, array_pr[i])) {
		    cerr <<c->get_filename()<<":"<<en->get_line_number()<<":"<<en->get_name()<<" type mismatch"<<endl;
		    semantic_trav_error_counter++;
		}
		size++;
	    }
	    t_pr = array_pr[size];
	    t = e->get_type_name();
	    if (t == SELF_TYPE) 
		cerr << "Line#" << e->get_line_number() << ": " << e->get_name() << " in " << c->get_type() << "return type cannot be SELF_TYPE.\n ";
	    if (!ct->check_conformance(t0, t)) {
		cerr <<c->get_filename()<<":"<<e->get_line_number()<<":"<<e->get_name()<<"'s type "<<t0<<" does not conform to "<<t<<endl;
		semantic_trav_error_counter++;
	    }
	    if (t_pr == SELF_TYPE) {
		if (compareSelf(t_pr))
		    e->set_type(SELF_TYPE);
		else
		    e->set_type(idtable.add_string(t0->get_string()));
		return t0;
	    } else {
		if (compareSelf(t_pr))
		    e->set_type(SELF_TYPE);
		else
		    e->set_type(idtable.add_string(t_pr->get_string()));
		return t_pr;
	    }
    } /* condition */
    else if (e->get_kind() == 10) {
	t0 = check_types(ObjEnv, MetEnv, ct, c, e->get_pred());
	if (Bool != t0)
	    cerr << "#" << e->get_pred()->get_line_number() << ": condition predicate type mismatch\n";
	Symbol t2 = check_types(ObjEnv, MetEnv, ct, c, e->get_then_exp());
	Symbol t3 = check_types(ObjEnv, MetEnv, ct, c, e->get_else_exp());
	Symbol common_ancestor = ct->lub(t2, t3);
	e->set_type(idtable.add_string(common_ancestor->get_string()));
	return common_ancestor;
    } /* let */
    else if (e->get_kind() == 11) {
	if (self == e->get_name()) {
	    cerr << c->get_filename() << ":" << e->get_line_number() << ":" << "cannot assign to self in a let\n";
	    semantic_trav_error_counter++;
	    cerr << "Compilation halted due to static semantic errors." << endl;
	    exit(1);
	}
	t0 = e->get_type_name();
	if (t0 == SELF_TYPE) 
	    t0_pr = appendString(SELF_TYPE, c->get_type());
	else
	    t0_pr = t0;
	/* with initial expression */
	if (e->get_expr_init()->get_kind()) {
	    t1 = check_types(ObjEnv, MetEnv, ct, c, e->get_expr_init());
	    if (!ct->check_conformance(t1, t0_pr)) {
		cerr << c->get_filename() << ":"<< e->get_expr_init()->get_line_number() << ": infered type " << t1 << " does not conform to " << t0_pr << "\n";
		semantic_trav_error_counter++;
	    }
	} 
	ObjEnv->enterscope();
	ObjEnv->addid(e->get_name(), new Symbol(t0_pr));
	Symbol t2 = check_types(ObjEnv, MetEnv, ct, c, e->get_expr_body());
	ObjEnv->exitscope();
	if (compareSelf(t2)) 
	    e->set_type(SELF_TYPE);
	else
	    e->set_type(idtable.add_string(t2->get_string()));
	return t2;
    } /* case */
    else if (e->get_kind() == 12) {
	t0 = check_types(ObjEnv, MetEnv, ct, c, e->get_expr());
	Cases cs = e->get_cases();
	Symbol *duptable = new Symbol[cs->len()];
	Symbol common_ancestor = NULL;
	for (int i = cs->first(); cs->more(i); i = cs->next(i)) {
	    Case ca = cs->nth(i);
	    ObjEnv->enterscope();
	    ObjEnv->addid(ca->get_name(), new Symbol(ca->get_type()));
	    t_pr = check_types(ObjEnv, MetEnv, ct, c, ca->get_expr());
	    for (int j = 0; j < i; j++) {
		if (t_pr == duptable[j]) {
		    cerr << c->get_filename() <<":"<< ca->get_line_number() << "Same type " << t_pr << endl;
		    semantic_trav_error_counter++;
		}
	    }
	    duptable[i] = t_pr;
	    ObjEnv->exitscope();
	    if (!i) 
		common_ancestor = t_pr;
	    else
		common_ancestor = ct->lub(t_pr, common_ancestor);
	}
	e->set_type(idtable.add_string(common_ancestor->get_string()));
	return common_ancestor;
    } /* loop */
    else if (e->get_kind() == 13) {
	if (Bool != check_types(ObjEnv, MetEnv, ct, c, e->get_expr_pred())) {
	    cerr << c->get_filename() << ":" << e->get_expr_pred()->get_line_number() << ": Loop predicate type mismatch\n";
	    semantic_trav_error_counter++;
	}
	check_types(ObjEnv, MetEnv, ct, c, e->get_expr_body());
	e->set_type(Object);
	return Object;
    } /* isvoid */
    else if (e->get_kind() == 14) {
	check_types(ObjEnv, MetEnv, ct, c, e->get_expr());
	e->set_type(Bool);
	return Bool;
    } /* not */
    else if (e->get_kind() == 15) {
	if (Bool != check_types(ObjEnv, MetEnv, ct, c, e->get_expr()))
	    cerr << "#" << e->get_expr()->get_line_number() << ": Not type mismatch\n";
	e->set_type(Bool);
	return Bool;
    } /* compare */
    else if (e->get_kind() == 16 || e->get_kind() == 17) {
	bool is_error = false;
	if (Int != check_types(ObjEnv, MetEnv, ct, c, e->get_e1())) {
	    cerr << "#" << e->get_e1()->get_line_number() << ": Compare type mismatch\n";
	    semantic_trav_error_counter++;
	    is_error = true;
	}
 	if (Int != check_types(ObjEnv, MetEnv, ct, c, e->get_e2())) {
	    cerr << "#" << e->get_e2()->get_line_number() << ": Compare type mismatch\n";
	    semantic_trav_error_counter++;
	    is_error = true;
	}
	if (is_error) { e->set_type(Object); return Object; }
	e->set_type(Bool);
	return Bool;
    } /* neg */
    else if (e->get_kind() == 18) {
	if (Int != check_types(ObjEnv, MetEnv, ct, c, e->get_e1())){
	    cerr << "#" << e->get_e1()->get_line_number() << ": Negative type mismatch\n";
	    semantic_trav_error_counter++;
	    return Object;
	}
	e->set_type(idtable.add_string("Int"));
	return Int;
    } /* arith */
    else if (e->get_kind() >= 19 && e->get_kind() <= 22) {
 	if (Int != check_types(ObjEnv, MetEnv, ct, c, e->get_e1())) {
	    cerr << c->get_filename() << ":" << e->get_e1()->get_line_number() << ": Arith type mismatch\n";
	    semantic_trav_error_counter++;
	    e->set_type(Object);
	} else 
	    e->set_type(Int);
 	if (Int != check_types(ObjEnv, MetEnv, ct, c, e->get_e2())) {
	    cerr << c->get_filename() << ":" << e->get_e2()->get_line_number() << ": Arith type mismatch\n";
	    semantic_trav_error_counter++;
	    e->set_type(Object);
	} else 
	    e->set_type(Int);
	return Int;
    } /* equal */
    else if (e->get_kind() == 23) {
	t0 = check_types(ObjEnv, MetEnv, ct, c, e->get_e1());
	t1 = check_types(ObjEnv, MetEnv, ct, c, e->get_e2());
	if (t0 == Bool || t0 == Int || t0 == Str ||
	    t1 == Bool || t1 == Int || t1 == Str) {
	    if (t0 != t1) {
		cerr << c->get_filename() << ":" << e->get_e1()->get_line_number() << ": Equal type mismatch\n";
		semantic_trav_error_counter++;
		e->set_type(Object);
		return Object;
	    }
	}
	e->set_type(Bool);
	return Bool;
    }
    return Object;
}
bool ClassTable::check_conformance(Symbol leaf, Symbol root) {
    Symbol a, b; 
    if (compareSelf(leaf)) 
	a = partSelfString(leaf);
    else
	a = leaf;
    if (compareSelf(root)) 
	b = partSelfString(root);
    else 
	b = root;
    Elem *child = search(inGraph, a);
    Elem *elem_parent = search(inGraph, b);
    while (child) {
	if (child == elem_parent) return true;
	child = child->parent;
    }
    return false;
}
void TypesMatrix::grow() {
    TypesArray **new_array;
    int new_size;

    new_size = size * 2;
    new_array = new TypesArray*[new_size]();

    for (int i = 0; i < size; i++) 
	new_array[i] = array[i];
    delete array;
    array = new_array;
    size = new_size;
}
void TypesMatrix::push_back(Symbol *types, Symbol name, int size) {
    if (length == size)
	grow();
    TypesArray *t = new TypesArray();
    t->array = types, t->name = name, t->size = size;
    array[length++] = t;
}
Symbol *TypesMatrix::find(Symbol name, ClassTable *ct) {
    Symbol id = partFormer(name);
    Symbol cl = partLater(name);
    Elem *elem = ct->find(cl);
    while (elem) {
	name = appendString(id, elem->key);
	for (int i = 0; i < length; i++) {
	    if(compare2(array[i]->name ,name)) {
		return array[i]->array;
	    }
	}
	elem = elem->parent;
    }
    return NULL;
}

const float ClassTable::max_load_factor = 0.7;
unsigned int ClassTable::Hash(Symbol s) {
    unsigned int i;
    unsigned int c;
    unsigned int hash;
    unsigned int prime;

    hash = 5381;
    prime = 16777619;
    unsigned int string_len = s->get_len();
    char *str = s->get_string();
    for (i = 0; i < string_len; i++) {
	c = str[i];
	hash = (hash ^ c) * prime;
    }
    return hash;
}

void ClassTable::Grow(HashTable *ht) {
	int old_size;
	int index;
	int i;

	Elem **old_array;
	Elem *elem;

	old_size = ht->size;
	old_array = ht->array;

	ht->size = old_size * 2;
	ht->array = new Elem*[ht->size]();

	for (i = 0; i < old_size; i++) {
		while ((elem = old_array[i])) {
			old_array[i] = elem->next;

			index = Hash(elem->key) % ht->size;
			elem->next = ht->array[index];
			ht->array[index] = elem;
		}
	}

	delete[] old_array;
}

void ClassTable::insert(HashTable *ht, Symbol key, Class_ vertex, Elem *parent) {
    if (ht->length > ht->size * max_load_factor)
	Grow(ht);
    if (search(ht, key)) return;

    Elem *elem = new Elem;
    elem->key = key;
    elem->vertex = vertex;
    elem->parent = parent;

    unsigned int index = Hash(key) % ht->size;
    elem->next = ht->array[index];
    ht->array[index] = elem;
    ht->length++;
}

Elem *ClassTable::search(HashTable *ht, Symbol key) {
    unsigned int index;
    Elem *elem;

    if (!key) return NULL;
    index = Hash(key) % ht->size;
    for (elem = ht->array[index]; elem; elem = elem->next) {
	if (elem && (key->equal_string(elem->key->get_string(), elem->key->get_len()))) break;
    }

    return elem;
}
void ClassTable::del(HashTable *ht, Symbol key) {
    unsigned int index = Hash(key) % ht->size;

    Elem *prev = NULL;
    Elem *elem = ht->array[index];
    while (elem && !(key->equal_string(elem->key->get_string(), elem->key->get_len()))) {
	prev = elem;
	elem = elem->next;
    }
    if (!elem) return;
    if (prev)
	prev->next = elem->next;
    else
	ht->array[index] = elem->next;
    ht->length--;
    delete elem;
}
Symbol ClassTable::lub(Symbol t1, Symbol t2) {
    Elem *ancestor = search(inGraph, t1);
    while (ancestor) {
	Elem *elem = search(inGraph, t2);
	while (elem) {
	    if (elem->key == ancestor->key) return ancestor->key;
	    elem = elem->parent;
	}
	ancestor = ancestor->parent;
    }
    return NULL;
}
