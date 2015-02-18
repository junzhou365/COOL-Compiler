
//**************************************************************
//
// Code generator SKELETON
//
// Read the comments carefully. Make sure to
//    initialize the base class tags in
//       `CgenClassTable::CgenClassTable'
//
//    Add the label for the dispatch tables to
//       `IntEntry::code_def'
//       `StringEntry::code_def'
//       `BoolConst::code_def'
//
//    Add code to emit everyting else that is needed
//       in `CgenClassTable::code'
//
//
// The files as provided will produce code to begin the code
// segments, declare globals, and emit constants.  You must
// fill in the rest.
//
//**************************************************************

#include "cgen.h"
#include "cgen_gc.h"

extern void emit_string_constant(ostream& str, char *s);
extern Symbol concatSymbol(Symbol s1, Symbol s2);
extern int cgen_debug;

static int label_index = 0;
void restoreAllReg(ostream& s); 
static CgenClassTableP context;
//
// Three symbols from the semantic analyzer (semant.cc) are used.
// If e : No_type, then no code is generated for e.
// Special code is generated for new SELF_TYPE.
// The name "self" also generates code different from other references.
//
//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
Symbol 
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

static char *gc_init_names[] =
  { "_NoGC_Init", "_GenGC_Init", "_ScnGC_Init" };
static char *gc_collect_names[] =
  { "_NoGC_Collect", "_GenGC_Collect", "_ScnGC_Collect" };


//  BoolConst is a class that implements code generation for operations
//  on the two booleans, which are given global names here.
BoolConst falsebool(FALSE);
BoolConst truebool(TRUE);

//*********************************************************
//
// Define method for code generation
//
// This is the method called by the compiler driver
// `cgtest.cc'. cgen takes an `ostream' to which the assembly will be
// emmitted, and it passes this and the class list of the
// code generator tree to the constructor for `CgenClassTable'.
// That constructor performs all of the work of the code
// generator.
//
//*********************************************************

void program_class::cgen(ostream &os) 
{
  // spim wants comments to start with '#'
  os << "# start of generated code\n";

  initialize_constants();
  CgenClassTable *codegen_classtable = new CgenClassTable(classes,os);

  os << "\n# end of generated code\n";
}


//////////////////////////////////////////////////////////////////////////////
//
//  emit_* procedures
//
//  emit_X  writes code for operation "X" to the output stream.
//  There is an emit_X for each opcode X, as well as emit_ functions
//  for generating names according to the naming conventions (see emit.h)
//  and calls to support functions defined in the trap handler.
//
//  Register names and addresses are passed as strings.  See `emit.h'
//  for symbolic names you can use to refer to the strings.
//
//////////////////////////////////////////////////////////////////////////////

static void emit_load(char *dest_reg, int offset, char *source_reg, ostream& s)
{
  s << LW << dest_reg << " " << offset * WORD_SIZE << "(" << source_reg << ")" 
    << endl;
}

static void emit_store(char *source_reg, int offset, char *dest_reg, ostream& s)
{
  s << SW << source_reg << " " << offset * WORD_SIZE << "(" << dest_reg << ")"
      << endl;
}

static void emit_load_imm(char *dest_reg, int val, ostream& s)
{ s << LI << dest_reg << " " << val << endl; }

static void emit_load_address(char *dest_reg, char *address, ostream& s)
{ s << LA << dest_reg << " " << address << endl; }

static void emit_partial_load_address(char *dest_reg, ostream& s)
{ s << LA << dest_reg << " "; }

static void emit_load_bool(char *dest, const BoolConst& b, ostream& s)
{
  emit_partial_load_address(dest,s);
  b.code_ref(s);
  s << endl;
}

static void emit_load_string(char *dest, StringEntry *str, ostream& s)
{
  emit_partial_load_address(dest,s);
  str->code_ref(s);
  s << endl;
}

static void emit_load_int(char *dest, IntEntry *i, ostream& s)
{
  emit_partial_load_address(dest,s);
  i->code_ref(s);
  s << endl;
}

static void emit_move(char *dest_reg, char *source_reg, ostream& s)
{ s << MOVE << dest_reg << " " << source_reg << endl; }

static void emit_neg(char *dest, char *src1, ostream& s)
{ s << NEG << dest << " " << src1 << endl; }

static void emit_add(char *dest, char *src1, char *src2, ostream& s)
{ s << ADD << dest << " " << src1 << " " << src2 << endl; }

static void emit_addi(char *dest, char *src1, int imm, ostream& s)
{ s << ADDI << dest << " " << src1 << " " << imm << endl; }

static void emit_addu(char *dest, char *src1, char *src2, ostream& s)
{ s << ADDU << dest << " " << src1 << " " << src2 << endl; }

static void emit_addiu(char *dest, char *src1, int imm, ostream& s)
{ s << ADDIU << dest << " " << src1 << " " << imm << endl; }

static void emit_div(char *dest, char *src1, char *src2, ostream& s)
{ s << DIV << dest << " " << src1 << " " << src2 << endl; }

static void emit_mul(char *dest, char *src1, char *src2, ostream& s)
{ s << MUL << dest << " " << src1 << " " << src2 << endl; }

static void emit_sub(char *dest, char *src1, char *src2, ostream& s)
{ s << SUB << dest << " " << src1 << " " << src2 << endl; }

static void emit_sll(char *dest, char *src1, int num, ostream& s)
{ s << SLL << dest << " " << src1 << " " << num << endl; }

static void emit_jalr(char *dest, ostream& s)
{ s << JALR << "\t" << dest << endl; }

static void emit_jal(char *address,ostream &s)
{ s << JAL << address << endl; }

static void emit_return(ostream& s)
{ s << RET << endl; }

static void emit_gc_assign(ostream& s)
{ s << JAL << "_GenGC_Assign" << endl; }

static void emit_disptable_ref(Symbol sym, ostream& s)
{  s << sym << DISPTAB_SUFFIX; }

static void emit_init_ref(Symbol sym, ostream& s)
{ s << sym << CLASSINIT_SUFFIX; }

static void emit_label_ref(int l, ostream &s)
{ s << "label" << l; }

static void emit_protobj_ref(Symbol sym, ostream& s)
{ s << sym << PROTOBJ_SUFFIX; }

static void emit_method_ref(Symbol classname, Symbol methodname, ostream& s)
{ s << classname << METHOD_SEP << methodname; }

static void emit_label_def(int l, ostream &s)
{
  emit_label_ref(l,s);
  s << ":" << endl;
}

static void emit_beqz(char *source, int label, ostream &s)
{
  s << BEQZ << source << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_beq(char *src1, char *src2, int label, ostream &s)
{
  s << BEQ << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bne(char *src1, char *src2, int label, ostream &s)
{
  s << BNE << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bleq(char *src1, char *src2, int label, ostream &s)
{
  s << BLEQ << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_blt(char *src1, char *src2, int label, ostream &s)
{
  s << BLT << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_blti(char *src1, int imm, int label, ostream &s)
{
  s << BLT << src1 << " " << imm << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bgti(char *src1, int imm, int label, ostream &s)
{
  s << BGT << src1 << " " << imm << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_branch(int l, ostream& s)
{
  s << BRANCH;
  emit_label_ref(l,s);
  s << endl;
}

//
// Push a register on the stack. The stack grows towards smaller addresses.
//
static void emit_push(char *reg, ostream& str)
{
  emit_store(reg,0,SP,str);
  emit_addiu(SP,SP,-4,str);
}

//
// Fetch the integer value in an Int object.
// Emits code to fetch the integer value of the Integer object pointed
// to by register source into the register dest
//
static void emit_fetch_int(char *dest, char *source, ostream& s)
{ emit_load(dest, DEFAULT_OBJFIELDS, source, s); }

//
// Emits code to store the integer value contained in register source
// into the Integer object pointed to by dest.
//
static void emit_store_int(char *source, char *dest, ostream& s)
{ emit_store(source, DEFAULT_OBJFIELDS, dest, s); }


static void emit_test_collector(ostream &s)
{
  emit_push(ACC, s);
  emit_move(ACC, SP, s); // stack end
  emit_move(A1, ZERO, s); // allocate nothing
  s << JAL << gc_collect_names[cgen_Memmgr] << endl;
  emit_addiu(SP,SP,4,s);
  emit_load(ACC,0,SP,s);
}

static void emit_gc_check(char *source, ostream &s)
{
  if (source != (char*)A1) emit_move(A1, source, s);
  s << JAL << "_gc_check" << endl;
}

///////////////////////////////////////////////////////////////////////////////
//
// coding strings, ints, and booleans
//
// Cool has three kinds of constants: strings, ints, and booleans.
// This section defines code generation for each type.
//
// All string constants are listed in the global "stringtable" and have
// type StringEntry.  StringEntry methods are defined both for String
// constant definitions and references.
//
// All integer constants are listed in the global "inttable" and have
// type IntEntry.  IntEntry methods are defined for Int
// constant definitions and references.
//
// Since there are only two Bool values, there is no need for a table.
// The two booleans are represented by instances of the class BoolConst,
// which defines the definition and reference methods for Bools.
//
///////////////////////////////////////////////////////////////////////////////

//
// Strings
//
void StringEntry::code_ref(ostream& s)
{
  s << STRCONST_PREFIX << index;
}

//
// Emit code for a constant String.
// You should fill in the code naming the dispatch table.
//

void StringEntry::code_def(ostream& s, int stringclasstag)
{
  IntEntryP lensym = inttable.add_int(len);

  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);  s  << LABEL                                             // label
      << WORD << stringclasstag << endl                                 // tag
      << WORD << (DEFAULT_OBJFIELDS + STRING_SLOTS + (len+4)/4) << endl // size
      << WORD;


 /***** Add dispatch information for class String ******/

      s << "String" << DISPTAB_SUFFIX << endl;                                              // dispatch table
      s << WORD;  lensym->code_ref(s);  s << endl;            // string length
  emit_string_constant(s,str);                                // ascii string
  s << ALIGN;                                                 // align to word
}

//
// StrTable::code_string
// Generate a string object definition for every string constant in the 
// stringtable.
//
void StrTable::code_string_table(ostream& s, int stringclasstag)
{  
  for (List<StringEntry> *l = tbl; l; l = l->tl())
    l->hd()->code_def(s,stringclasstag);
}

//
// Ints
//
void IntEntry::code_ref(ostream &s)
{
  s << INTCONST_PREFIX << index;
}

//
// Emit code for a constant Integer.
// You should fill in the code naming the dispatch table.
//

void IntEntry::code_def(ostream &s, int intclasstag)
{
  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);  s << LABEL                                // label
      << WORD << intclasstag << endl                      // class tag
      << WORD << (DEFAULT_OBJFIELDS + INT_SLOTS) << endl  // object size
      << WORD; 

 /***** Add dispatch information for class Int ******/

      s << "Int" << DISPTAB_SUFFIX << endl;                                          // dispatch table
      s << WORD << str << endl;                           // integer value
}


//
// IntTable::code_string_table
// Generate an Int object definition for every Int constant in the
// inttable.
//
void IntTable::code_string_table(ostream &s, int intclasstag)
{
  for (List<IntEntry> *l = tbl; l; l = l->tl())
    l->hd()->code_def(s,intclasstag);
}


//
// Bools
//
BoolConst::BoolConst(int i) : val(i) { assert(i == 0 || i == 1); }

void BoolConst::code_ref(ostream& s) const
{
  s << BOOLCONST_PREFIX << val;
}
  
//
// Emit code for a constant Bool.
// You should fill in the code naming the dispatch table.
//

void BoolConst::code_def(ostream& s, int boolclasstag)
{
  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);  s << LABEL                                  // label
      << WORD << boolclasstag << endl                       // class tag
      << WORD << (DEFAULT_OBJFIELDS + BOOL_SLOTS) << endl   // object size
      << WORD;

 /***** Add dispatch information for class Bool ******/

      s << "Bool" << DISPTAB_SUFFIX << endl;                                            // dispatch table
      s << WORD << val << endl;                             // value (0 or 1)
}

//////////////////////////////////////////////////////////////////////////////
//
//  CgenClassTable methods
//
//////////////////////////////////////////////////////////////////////////////

//***************************************************
//
//  Emit code to start the .data segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_data()
{
  Symbol main    = idtable.lookup_string(MAINNAME);
  Symbol string  = idtable.lookup_string(STRINGNAME);
  Symbol integer = idtable.lookup_string(INTNAME);
  Symbol boolc   = idtable.lookup_string(BOOLNAME);

  str << "\t.data\n" << ALIGN;
  //
  // The following global names must be defined first.
  //
  str << GLOBAL << CLASSNAMETAB << endl;
  str << GLOBAL; emit_protobj_ref(main,str);    str << endl;
  str << GLOBAL; emit_protobj_ref(integer,str); str << endl;
  str << GLOBAL; emit_protobj_ref(string,str);  str << endl;
  str << GLOBAL; falsebool.code_ref(str);  str << endl;
  str << GLOBAL; truebool.code_ref(str);   str << endl;
  str << GLOBAL << INTTAG << endl;
  str << GLOBAL << BOOLTAG << endl;
  str << GLOBAL << STRINGTAG << endl;

  //
  // We also need to know the tag of the Int, String, and Bool classes
  // during code generation.
  //
  str << INTTAG << LABEL
      << WORD << intclasstag << endl;
  str << BOOLTAG << LABEL 
      << WORD << boolclasstag << endl;
  str << STRINGTAG << LABEL 
      << WORD << stringclasstag << endl;    
}


//***************************************************
//
//  Emit code to start the .text segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_text()
{
  str << GLOBAL << HEAP_START << endl
      << HEAP_START << LABEL 
      << WORD << 0 << endl
      << "\t.text" << endl
      << GLOBAL;
  emit_init_ref(idtable.add_string("Main"), str);
  str << endl << GLOBAL;
  emit_init_ref(idtable.add_string("Int"),str);
  str << endl << GLOBAL;
  emit_init_ref(idtable.add_string("String"),str);
  str << endl << GLOBAL;
  emit_init_ref(idtable.add_string("Bool"),str);
  str << endl << GLOBAL;
  emit_method_ref(idtable.add_string("Main"), idtable.add_string("main"), str);
  str << endl;
}

void CgenClassTable::code_bools(int boolclasstag)
{
  falsebool.code_def(str,boolclasstag);
  truebool.code_def(str,boolclasstag);
}

void CgenClassTable::code_select_gc()
{
  //
  // Generate GC choice constants (pointers to GC functions)
  //
  str << GLOBAL << "_MemMgr_INITIALIZER" << endl;
  str << "_MemMgr_INITIALIZER:" << endl;
  str << WORD << gc_init_names[cgen_Memmgr] << endl;
  str << GLOBAL << "_MemMgr_COLLECTOR" << endl;
  str << "_MemMgr_COLLECTOR:" << endl;
  str << WORD << gc_collect_names[cgen_Memmgr] << endl;
  str << GLOBAL << "_MemMgr_TEST" << endl;
  str << "_MemMgr_TEST:" << endl;
  str << WORD << (cgen_Memmgr_Test == GC_TEST) << endl;
}


//********************************************************
//
// Emit code to reserve space for and initialize all of
// the constants.  Class names should have been added to
// the string table (in the supplied code, is is done
// during the construction of the inheritance graph), and
// code for emitting string constants as a side effect adds
// the string's length to the integer table.  The constants
// are emmitted by running through the stringtable and inttable
// and producing code for each entry.
//
//********************************************************

void CgenClassTable::code_constants()
{
  //
  // Add constants that are required by the code generator.
  //
  stringtable.add_string("");
  inttable.add_string("0");

  stringtable.code_string_table(str,stringclasstag);
  inttable.code_string_table(str,intclasstag);
  code_bools(boolclasstag);
}


CgenClassTable::CgenClassTable(Classes classes, ostream& s) : nds(NULL) , str(s)
{
   stringclasstag = 2 /* Change to your String class tag here */;
   intclasstag =    3 /* Change to your Int class tag here */;
   boolclasstag =   4 /* Change to your Bool class tag here */;
   classtag = 5;

   enterscope();
   if (cgen_debug) cout << "Building CgenClassTable" << endl;
   install_basic_classes();
   install_classes(classes);
   build_inheritance_tree();
   context = this;
   code();
   exitscope();
}

void CgenClassTable::install_basic_classes()
{

// The tree package uses these globals to annotate the classes built below.
  //curr_lineno  = 0;
  Symbol filename = stringtable.add_string("<basic class>");

//
// A few special class names are installed in the lookup table but not
// the class list.  Thus, these classes exist, but are not part of the
// inheritance hierarchy.
// No_class serves as the parent of Object and the other special classes.
// SELF_TYPE is the self class; it cannot be redefined or inherited.
// prim_slot is a class known to the code generator.
//
  addid(No_class,
	new CgenNode(class_(No_class,No_class,nil_Features(),filename),
			    Basic,this));
  addid(SELF_TYPE,
	new CgenNode(class_(SELF_TYPE,No_class,nil_Features(),filename),
			    Basic,this));
  addid(prim_slot,
	new CgenNode(class_(prim_slot,No_class,nil_Features(),filename),
			    Basic,this));

// 
// The Object class has no parent class. Its methods are
//        cool_abort() : Object    aborts the program
//        type_name() : Str        returns a string representation of class name
//        copy() : SELF_TYPE       returns a copy of the object
//
// There is no need for method bodies in the basic classes---these
// are already built in to the runtime system.
//
  install_class(
   new CgenNode(
    class_(Object, 
	   No_class,
	   append_Features(
           append_Features(
           single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
           single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
           single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	   filename),
    Basic,this));

// 
// The IO class inherits from Object. Its methods are
//        out_string(Str) : SELF_TYPE          writes a string to the output
//        out_int(Int) : SELF_TYPE               "    an int    "  "     "
//        in_string() : Str                    reads a string from the input
//        in_int() : Int                         "   an int     "  "     "
//
   install_class(
    new CgenNode(
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
	   filename),	    
    Basic,this));

//
// The Int class has no methods and only a single attribute, the
// "val" for the integer. 
//
   install_class(
    new CgenNode(
     class_(Int, 
	    Object,
            single_Features(attr(val, prim_slot, no_expr())),
	    filename),
     Basic,this));

//
// Bool also has only the "val" slot.
//
    install_class(
     new CgenNode(
      class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename),
      Basic,this));

//
// The class Str has a number of slots and operations:
//       val                                  ???
//       str_field                            the string itself
//       length() : Int                       length of the string
//       concat(arg: Str) : Str               string concatenation
//       substr(arg: Int, arg2: Int): Str     substring
//       
   install_class(
    new CgenNode(
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
	     filename),
        Basic,this));

}

// CgenClassTable::install_class
// CgenClassTable::install_classes
//
// install_classes enters a list of classes in the symbol table.
//
void CgenClassTable::install_class(CgenNodeP nd)
{
  Symbol name = nd->get_name();

  if (probe(name))
    {
      return;
    }

  // The class name is legal, so add it to the list of classes
  // and the symbol table.
  nds = new List<CgenNode>(nd,nds);
  addid(name,nd);
}

void CgenClassTable::install_classes(Classes cs)
{
  for(int i = cs->first(); cs->more(i); i = cs->next(i))
    install_class(new CgenNode(cs->nth(i),NotBasic,this));
}

//
// CgenClassTable::build_inheritance_tree
//
void CgenClassTable::build_inheritance_tree()
{
  for(List<CgenNode> *l = nds; l; l = l->tl())
      set_relations(l->hd());
}

//
// CgenClassTable::set_relations
//
// Takes a CgenNode and locates its, and its parent's, inheritance nodes
// via the class table.  Parent and child pointers are added as appropriate.
//
void CgenClassTable::set_relations(CgenNodeP nd)
{
  CgenNode *parent_node = probe(nd->get_parent());
  nd->set_parentnd(parent_node);
  parent_node->add_child(nd);
}

void CgenNode::add_child(CgenNodeP n)
{
  children = new List<CgenNode>(n,children);
}

void CgenNode::set_parentnd(CgenNodeP p)
{
  assert(parentnd == NULL);
  assert(p != NULL);
  parentnd = p;
}



void CgenClassTable::code()
{
  if (cgen_debug) cout << "coding global data" << endl;
  code_global_data();

  if (cgen_debug) cout << "choosing gc" << endl;
  code_select_gc();

  if (cgen_debug) cout << "coding constants" << endl;
  code_constants();

//                 Add your code to emit
//                   - prototype objects
//                   - class_nameTab
//                   - dispatch tables
//

  // Add class_nameTab
  str << CLASSNAMETAB << LABEL;
  int class_nameTabSize = list_length(nds);
  int stringtableSize = 0;
  for (stringtableSize = stringtable.first(); stringtable.more(stringtableSize); stringtableSize = stringtable.next(stringtableSize));
  for (int i = stringtableSize - class_nameTabSize - 1; i < stringtableSize - 1; i++) 
	str << WORD <<  STRCONST_PREFIX << i << endl;
 
  // Add class_objTab
  str << CLASSOBJTAB << LABEL;
  dfsPrintObjTab(root());
  

  // prototype objects
  List<Entry> *attrList = NULL;
  dfsUpdateProtObj(root(), attrList, NULL);

  // dispatch tables
  List<Entry> *methodList = NULL;
  dfsUpdateDispatchTab(root(), methodList, NULL);

  if (cgen_debug) cout << "coding global text" << endl;
  code_global_text();

//                 Add your code to emit
//                   - object initializer
//                   - the class methods
//                   - etc...
  dfsUpdateInits(root());
  dfsUpdateMethods(root());
  
}

void CgenClassTable::dfsUpdateMethods(CgenNodeP node) {
    if (!node) return;
    if (!node->basic())
	node->code_method(str);
    for (List<CgenNode> *l = node->get_children(); l; l = l->tl()) {
	    dfsUpdateMethods(l->hd());
    }
}

void CgenNode::code_method(ostream& s) {
    for (int i = features->first(); features->more(i); i = features->next(i)) {
	Feature f = features->nth(i);
	if (f->get_kind() == method_k) {
	    emit_method_ref(name, f->get_name(), s);s << LABEL;
	    // store the following items in the following order
	    // fp
	    // self
	    // return address
	    emit_addiu(SP, SP, -12, s);
	    emit_store(FP, 3, SP, s);
	    emit_store(SELF, 2, SP, s);
	    emit_store(RA, 1, SP, s);
	    // fp points to sp + 4, which is the top of the stack
	    emit_addiu(FP, SP, 4, s);
	    // set the input as the current self
	    // i.e. update the self for evaluation
	    emit_move(SELF, ACC, s);
	    // output will be in the $a0 after next executes
	    f->get_expr()->code(s, env, met, f);

	    // restore fp, self, return address
	    emit_load(FP, 3, SP, s);
	    emit_load(SELF, 2, SP, s);
	    emit_load(RA, 1, SP, s);
	    int para_len = 0;
	    Formals fls = f->get_formals();
	    for (int i = fls->first(); fls->more(i); i = fls->next(i)) 
		para_len++;
	    emit_addiu(SP, SP, 12 + para_len * 4, s);
	    emit_return(s);
	}
  }

}
void CgenClassTable::dfsUpdateInits(CgenNodeP node) {
    if (!node) return;
    node->code_init(str);
    for (List<CgenNode> *l = node->get_children(); l; l = l->tl()) {
	dfsUpdateInits(l->hd());
    }
}
// input:	$a0 -- points to an uninitialized object

// output:	$a0 -- points to an initialized object
void CgenNode::code_init(ostream& s) {
    s << name << CLASSINIT_SUFFIX << LABEL;
    // store the following items in the following order
    // fp
    // self
    // return address
    emit_addiu(SP, SP, -12, s);
    emit_store(FP, 3, SP, s);
    emit_store(SELF, 2, SP, s);
    emit_store(RA, 1, SP, s);
    // fp points to sp + 4, which is the top of the stack
    emit_addiu(FP, SP, 4, s);
    // set the input as the current self
    // i.e. update the self for evaluation
    emit_move(SELF, ACC, s);
    // Initialize the current object's parent 
    if (parentnd->name != idtable.lookup_string("_no_class")) {
	Symbol parentSym = concatSymbol(parentnd->name, new Entry(CLASSINIT_SUFFIX, 5, 0));
	emit_jal(parentSym->get_string(), s);
    }

    // initialize attributes
    for (int i = features->first(); features->more(i); i = features->next(i)) {
	Feature f = features->nth(i);
	if (f->get_kind() == attr_k) {
	    f->get_expr()->code(s, env, met, f);
	    emit_store(ACC, ATTR_OFFSET + i, SELF, s);
	}
    }
    // move self to the $a0 which is the output
    emit_move(ACC, SELF, s);
    // restore fp, self, return address
    emit_load(FP, 3, SP, s);
    emit_load(SELF, 2, SP, s);
    emit_load(RA, 1, SP, s);
    emit_addiu(SP, SP, 12, s);
    emit_return(s);
}

void CgenClassTable::dfsPrintObjTab(CgenNodeP node) {
    if (!node) return;
    str << WORD << node->get_name() << PROTOBJ_SUFFIX << endl;
    str << WORD << node->get_name() << CLASSINIT_SUFFIX << endl;
    for (List<CgenNode> *l = node->get_children(); l; l = l->tl()) 
	dfsPrintObjTab(l->hd());
}

void CgenClassTable::dfsUpdateDispatchTab(CgenNodeP node, List<Entry> *methodList, List<Entry> *met) {
    if (!node) return;
    methodList = node->code_dispatchTab(str, methodList, met);
    met = node->get_met();
    List<Entry> *backUpList = methodList, *backUpMetList = met;
    for (List<CgenNode> *l = node->get_children(); l; l = l->tl()) {
	dfsUpdateDispatchTab(l->hd(), methodList, met);
	methodList = backUpList, met = backUpMetList;
    }
}

List<Entry> *CgenNode::code_dispatchTab(ostream& s, List<Entry> *methodList, List<Entry> *met) {
    this->met = met;
    Symbol nMethodDot = concatSymbol(name, new Entry(METHOD_SEP, 1, 0));
    for (int i = features->first(); features->more(i); i = features->next(i)) {
	Feature f = features->nth(i);
	if (f->get_kind() == method_k) {
	    Symbol nMethod = concatSymbol(nMethodDot, f->get_name());
	    methodList = new List<Entry>(nMethod, methodList);
	    this->met = new List<Entry>(f->get_name(), this->met);
	}
    }
    s << name << DISPTAB_SUFFIX << LABEL;
    for (List<Entry> *l = methodList; l; l = l->tl()) {
	s << WORD << l->hd() << endl;
    }
    return methodList;
}

void CgenClassTable::dfsUpdateProtObj(CgenNodeP node, List<Entry> *attrList, List<Entry> *env) {
    if (!node) return;
    if (node->basic()) 
	node->code_basic_protobj(str, intclasstag, boolclasstag, stringclasstag);
    else {
	attrList = node->code_protobj(str, classtag, attrList, env); 
	env = node->get_env();
	classtag++;
    }
    List<Entry> *backUpAttrList = attrList;
    List<Entry> *backUpEnv = env;
    for (List<CgenNode> *l = node->get_children(); l; l = l->tl()) {
	dfsUpdateProtObj(l->hd(), attrList, env);
	attrList = backUpAttrList;
	env = backUpEnv;
    }
}
void CgenNode::code_basic_protobj(ostream &s, int intclasstag, int boolclasstag, int stringclasstag) {
    if (name == Int) {
	s << WORD << "-1" << endl;
	s << name << PROTOBJ_SUFFIX << LABEL;
	s << WORD << intclasstag << endl;
	s << WORD << (DEFAULT_OBJFIELDS + INT_SLOTS) << endl; // size
	s << WORD << name << DISPTAB_SUFFIX << endl;	// dispatch
	s << WORD << "0" << endl;	// value, default 0
    } else if (name == Bool) {
	s << WORD << "-1" << endl;
	s << name << PROTOBJ_SUFFIX << LABEL;
	s << WORD << boolclasstag << endl;
	s << WORD << (DEFAULT_OBJFIELDS + BOOL_SLOTS) << endl; // size
	s << WORD << name << DISPTAB_SUFFIX << endl;	// dispatch
	s << WORD << "0" << endl;	// value, default false
    } else if (name == Str) {
	s << WORD << "-1" << endl;
	s << name << PROTOBJ_SUFFIX << LABEL;
	s << WORD << stringclasstag << endl;
	s << WORD << (DEFAULT_OBJFIELDS + STRING_SLOTS) << endl; // size
	s << WORD << name << DISPTAB_SUFFIX << endl;	// dispatch
	s << WORD << "0" << endl;	// value, default false
    } else if (name == Object) {
	s << WORD << "-1" << endl;
	s << name << PROTOBJ_SUFFIX << LABEL;
	s << WORD << "0" << endl;
	s << WORD << (DEFAULT_OBJFIELDS) << endl; // size
	s << WORD << name << DISPTAB_SUFFIX << endl;	// dispatch
    } else if (name == IO) {
	s << WORD << "-1" << endl;
	s << name << PROTOBJ_SUFFIX << LABEL;
	s << WORD << "1" << endl;
	s << WORD << (DEFAULT_OBJFIELDS) << endl; // size
	s << WORD << name << DISPTAB_SUFFIX << endl;	// dispatch
    }
}
List<Entry> *CgenNode::code_protobj(ostream &s, int classtag, List<Entry> *attrList, List<Entry> *env) {
    s << WORD << "-1" << endl;
    s << name << PROTOBJ_SUFFIX << LABEL;
    s << WORD << classtag << endl;
    int attr_size = list_length(attrList);
    for (int i = features->first(); features->more(i); i = features->next(i)) {
	Feature f = features->nth(i);
	if (f->get_kind() == attr_k)  attr_size++;
    }
    s << WORD << (DEFAULT_OBJFIELDS + attr_size) << endl; // size
    s << WORD << name << DISPTAB_SUFFIX << endl;	// dispatch
   // What if the inherited attr has the same name as the attr has?
   // emit attributes
   this->env = env;
    List<Entry> *nAttrList = NULL;
    for (int i = features->first(); features->more(i); i = features->next(i)) {
	Feature f = features->nth(i);
	if (f->get_kind() == attr_k) {
	    Symbol nAttr = NULL;
	    if (f->get_expr()->get_kind() == int_const_k) {
		nAttr = new Entry(INT_CONST0, INT_CONST0_LEN, 0);
		s << WORD << nAttr << endl;
		nAttrList = new List<Entry>(nAttr, nAttrList);
	    } else if (f->get_expr()->get_kind() == string_const_k) {
		nAttr = new Entry(STRING_CONST0, STRING_CONST0_LEN, 0);
		s << WORD << nAttr << endl;
		nAttrList = new List<Entry>(nAttr, nAttrList);
	    } else if (f->get_expr()->get_kind() == bool_const_k) {
		nAttr = new Entry(BOOL_CONST0, BOOL_CONST0_LEN, 0);
		s << WORD << nAttr << endl;
		nAttrList = new List<Entry>(nAttr, nAttrList);
	    } else {
		nAttr = new Entry(VOID, VOID_SIZE, 0);
		s << WORD << VOID << endl;	
		nAttrList = new List<Entry>(nAttr, nAttrList);
	    } 
	    this->env = new List<Entry>(f->get_name(), this->env);
	}
    }
    for (List<Entry> *l = attrList; l; l = l->tl()) {
	s << WORD << l->hd() << endl;
    }
 
    return nAttrList;
}
CgenNodeP CgenClassTable::root()
{
   return probe(Object);
}


///////////////////////////////////////////////////////////////////////
//
// CgenNode methods
//
///////////////////////////////////////////////////////////////////////

CgenNode::CgenNode(Class_ nd, Basicness bstatus, CgenClassTableP ct) :
   class__class((const class__class &) *nd),
   parentnd(NULL),
   children(NULL),
   basic_status(bstatus)
{ 
   stringtable.add_string(name->get_string());          // Add class name to string table
}


//******************************************************************
//
//   Fill in the following methods to produce code for the
//   appropriate expression.  You may add or remove parameters
//   as you wish, but if you do, remember to change the parameters
//   of the declarations in `cool-tree.h'  Sample code for
//   constant integers, strings, and booleans are provided.
//
//*****************************************************************

void assign_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
    expr->code(s, env, met, f);
    // the result now is in the $a0
    int index = 0;
    int attr_len = list_length(env);
    for (List<Entry> *l = env; l; l = l->tl()){
	if (name == l->hd()) {
	    emit_store(ACC, attr_len - 1 - index + ATTR_OFFSET, SELF, s);
	    return;
	}
	index++;
    }
}

void static_dispatch_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
}
// output:	$a0
void dispatch_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
    // we first keep the env and met
    // the size and the order of the parameters are the same 
    // as the formals
    for (int i = actual->first(); actual->more(i); i = actual->next(i)) {
	actual->nth(i)->code(s, env, met, f);
	// we need to save the result onto stack
	emit_addiu(SP, SP, -4, s);
	emit_store(ACC, 1, SP, s);
    }
    // the output of this is in the $a0; it is the self.
    expr->code(s, env, met, f);
    // after new self has been feched, we need to update the env and met
    env = (context->probe(expr->get_type()))->get_env();
    met = (context->probe(expr->get_type()))->get_met();

    emit_bne(ACC, ZERO, label_index, s);
    emit_load_address(ACC, STRING_CONST0, s);
    emit_load_imm(T1, 1, s);
    emit_jal(DISPATCH_ABORT, s);
    emit_label_ref(label_index++, s); s << LABEL;
    // get dispatchTab entry
    emit_load(T1, DISPTABLE_OFFSET, ACC, s);
    // have to be careful about the offset of the dispatch table
    int index = 0;
    for (List<Entry> *l = met; l; l = l->tl()) {
	if (name == l->hd()) 
	    break;
	index++;
    }
    emit_load(T1, index, T1, s);
    // next it will jump to execute the corresponding method
    emit_jalr(T1, s);
}

void cond_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
}

void loop_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
}

void typcase_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
}

void block_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
    for(int i = body->first(); body->more(i); i = body->next(i)) {
	body->nth(i)->code(s, env, met, f);
    }
}

void let_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
}

// output:	$a0
void plus_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
    e1->code(s, env, met, f);
    // the output should be *int and the result is in the $a0
    // int_const: offset 12 is the integer
    emit_load(T1, INT_ATTR_OFFSET, ACC, s);
    emit_addiu(SP, SP, -4, s);
    emit_store(T1, 1, SP, s);

    e2->code(s, env, met, f);
    // we have to create an object in the heap, instead of modifying a constant
    // copy the prototype of int, which is in the $a0
    s << JAL; emit_method_ref(idtable.lookup_string("Object"), idtable.lookup_string("copy"), s); s << endl;
    emit_load(T2, INT_ATTR_OFFSET, ACC, s); 
    emit_load(T1, 1, SP, s);
    emit_addiu(SP, SP, 4, s);
    emit_add(T1, T1, T2, s);
    // save the result to newly created object, which is pointed by $a0
    emit_store(T1, INT_ATTR_OFFSET, ACC, s);
}

void sub_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
    e1->code(s, env, met, f);
    // the output should be *int and the result is in the $a0
    // int_const: offset 12 is the integer
    emit_load(T1, INT_ATTR_OFFSET, ACC, s);
    emit_addiu(SP, SP, -4, s);
    emit_store(T1, 1, SP, s);

    e2->code(s, env, met, f);
    // we have to create an object in the heap, instead of modifying a constant
    // copy the prototype of int, which is in the $a0
    s << JAL; emit_method_ref(idtable.lookup_string("Object"), idtable.lookup_string("copy"), s); s << endl;
    emit_load(T2, INT_ATTR_OFFSET, ACC, s); 
    emit_load(T1, 1, SP, s);
    emit_addiu(SP, SP, 4, s);
    emit_sub(T1, T1, T2, s);
    // save the result to newly created object, which is pointed by $a0
    emit_store(T1, INT_ATTR_OFFSET, ACC, s);
}

void mul_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
    e1->code(s, env, met, f);
    // the output should be *int and the result is in the $a0
    // int_const: offset 12 is the integer
    emit_load(T1, INT_ATTR_OFFSET, ACC, s);
    emit_addiu(SP, SP, -4, s);
    emit_store(T1, 1, SP, s);

    e2->code(s, env, met, f);
    // we have to create an object in the heap, instead of modifying a constant
    // copy the prototype of int, which is in the $a0
    s << JAL; emit_method_ref(idtable.lookup_string("Object"), idtable.lookup_string("copy"), s); s << endl;
    emit_load(T2, INT_ATTR_OFFSET, ACC, s); 
    emit_load(T1, 1, SP, s);
    emit_addiu(SP, SP, 4, s);
    emit_mul(T1, T1, T2, s);
    // save the result to newly created object, which is pointed by $a0
    emit_store(T1, INT_ATTR_OFFSET, ACC, s);
}

void divide_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
    e1->code(s, env, met, f);
    // the output should be *int and the result is in the $a0
    // int_const: offset 12 is the integer
    emit_load(T1, INT_ATTR_OFFSET, ACC, s);
    emit_addiu(SP, SP, -4, s);
    emit_store(T1, 1, SP, s);

    e2->code(s, env, met, f);
    // we have to create an object in the heap, instead of modifying a constant
    // copy the prototype of int, which is in the $a0
    s << JAL; emit_method_ref(idtable.lookup_string("Object"), idtable.lookup_string("copy"), s); s << endl;
    emit_load(T2, INT_ATTR_OFFSET, ACC, s); 
    emit_load(T1, 1, SP, s);
    emit_addiu(SP, SP, 4, s);
    emit_div(T1, T1, T2, s);
    // save the result to newly created object, which is pointed by $a0
    emit_store(T1, INT_ATTR_OFFSET, ACC, s);
}

void neg_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
}

void lt_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
}

void eq_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
}

void leq_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
}

void comp_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
}

void int_const_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f)  
{
  //
  // Need to be sure we have an IntEntry *, not an arbitrary Symbol
  //
  emit_load_int(ACC,inttable.lookup_string(token->get_string()),s);
}

void string_const_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f)
{
  emit_load_string(ACC,stringtable.lookup_string(token->get_string()),s);
}

void bool_const_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f)
{
  emit_load_bool(ACC, BoolConst(val), s);
}

void new__class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
    if (type_name == SELF_TYPE) {
	emit_move(ACC, SELF, s);
    }
    emit_load_address(ACC, concatSymbol(type_name, new Entry(PROTOBJ_SUFFIX, 8, 0))->get_string(), s);
    s << JAL; emit_method_ref(idtable.lookup_string("Object"), idtable.lookup_string("copy"), s); s << endl;
    s << JAL; emit_init_ref(type_name, s); s << endl;

}

void isvoid_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
}

void no_expr_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
}

// output:	$a0
void object_class::code(ostream& s, List<Entry> *env, List<Entry>* met, Feature f) {
    if (name == idtable.lookup_string("self")) {
	emit_move(ACC, SELF, s);
	return;
    }
    int index = 0;
    bool foundInAttr = false;
    int attr_len = list_length(env);
    for (List<Entry> *l = env; l; l = l->tl()){
	if (name == l->hd()) {
	    emit_load(ACC, attr_len-1-index + ATTR_OFFSET, SELF, s);
	    return;
	}
	index++;
    }
    int para_len = 0;
    Formals fls = f->get_formals();
    for (int i = fls->first(); fls->more(i); i = fls->next(i))
	para_len++;
    if (!foundInAttr) {
	Formals fls = f->get_formals();
	for (int i = fls->first(); fls->more(i); i = fls->next(i)) {
	    Formal fl = fls->nth(i);
	    if (name == fl->get_name()) {
		emit_load(ACC, para_len-i-1 + REGS_STORE_LEN, FP, s);
		return;
	    }
	}
    }
}

void restoreAllReg(ostream& s) {
    emit_load(FP, 3, SP, s);
    emit_load(SELF, 2, SP, s);
    emit_load(RA, 1, SP, s);
    emit_addiu(SP, SP, 12, s);
}

