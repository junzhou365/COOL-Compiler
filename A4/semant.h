#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>  
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include "list.h"

#define TRUE 1
#define FALSE 0
class ClassTable;
typedef ClassTable *ClassTableP;

// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.
struct Elem {
    Symbol key;
    Class_ vertex;
    Elem *parent;
    Elem *next;
};
class TypesMatrix {
    struct TypesArray {
	Symbol *array;
	Symbol name;
	int size;
    };
    TypesArray **array;
    int size;
    int length;
    void grow();
public:
    TypesMatrix() { size = 10, length = 0; array = new TypesArray*[size]();}
    void push_back(Symbol *types, Symbol name, int size); 
    int  get_len() { return length; }
    Symbol *find(Symbol name, ClassTable *ct);
};

class ClassTable {
private:
  int semant_errors;
  void install_basic_classes();
  ostream& error_stream;
  /* add hash table inside ClassTable */
  struct HashTable {
      Elem **array;
      int length;
      int size;
  };
  HashTable *inGraph, *nDefClasses;

  static const float max_load_factor;

  void Grow(HashTable *ht);
  unsigned int Hash(Symbol s);
  void insert(HashTable *ht, Symbol key, Class_ vertex, Elem *parent);
  Elem *search(HashTable *ht, Symbol key);
  void del(HashTable *ht, Symbol key);

public:
  ClassTable(Classes);
  ~ClassTable();
  int errors() { return semant_errors; }
  ostream& semant_error();
  ostream& semant_error(Class_ c);
  ostream& semant_error(Symbol filename, tree_node *t);
  bool check_conformance(Symbol, Symbol);
  Elem *find(Symbol key) { return search(inGraph, key); }
  Symbol lub(Symbol t1, Symbol t2);
};

#endif

