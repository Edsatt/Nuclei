#pragma once

#ifdef INTERP
#else
  #include "linked.c"
#endif
#include "specific.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TYPELENGTH 10
#define MAXTOKENSIZE 100
#define VARLENGTH 10
#define STACKSIZE 100
#define ALPHA 26
#define MAXNUMTOKENS 2000
#define MAX MAXNUMTOKENS*MAXTOKENSIZE
#define NIL NULL
#define ASCII_A 65
#define strsame(X,Y) strcmp(X,Y)==0

typedef struct token{
  int has_data;
  char type[TYPELENGTH];
  char value[MAXTOKENSIZE];
}Token;

typedef struct variable{
  int var_set;
  char name[VARLENGTH];
  char value[MAXTOKENSIZE];
}Variable;

typedef struct stack{
  char data[STACKSIZE][MAXTOKENSIZE];
  int sp;
}Stack;

typedef struct flags{
  int interp;
  int parse;
  int enable_error_messages;
  int parsing_failure;
  int interp_failure;
  char output_str[LISTSTRLEN];
  char output_int[VARLENGTH];
  int output_bool;
  int intfunc;
  int conditional;
}Flags;

typedef struct prog{
  Token tokens[MAXNUMTOKENS];
  Variable variables[ALPHA];
  int pc;
  Stack stack;
  Flags flags;
}Program;

void initialise_program(Program* p);
void initialise_stack(Program* p);
void interp_enable_check(Program* p);
char* file_name(char* input, char* type);
void file_tostring(FILE* file, char* str);
void string_clean(char* str, char* temp_str);
void tokenise(Program* p, char* str, int i);
bool char_sorter(char* str, int i, Program* p);
bool is_keyword(char* str, int i, Program* p);
void tokenise_stringorliteral(char* str, char* type, int i, Program* p);
void tokenise_singlechar(char* str, int i, Program* p);
bool tokenise_keyword(char* type, char* str, char* cmp_str, int i, Program* p);
bool store_token(char* token, char* value, Program* p);
bool Prog(Program* p);
void Instrcts(Program* p);
void Instrct(Program* p);
void Func(Program* p);
bool Retfunc(Program* p);
bool Iofunc(Program* p);
bool If_statement(Program* p);
void flip_output_bool(Program* p);
bool Loop(Program* p);
void Listfunc(Program* p);
void Intfunc(Program* p);
void Boolfunc(Program* p);
void List(Program* p);
void Set(Program* p);
void Print(Program* p);
void Interp_Print(Program* p,int pc);
void Interp_Set(Program* p, int pc);
char* bool_to_string(Program* p);
void store_variable(char* name, char* value, Program* p);
void Push(Program* p, int pc);
char* Pop(Program* p);
char* get_variable(Program* p, int pc);
void Interp_Cons(Program* p);
lisp* Interp_Cons_helper(lisp* l, Program* p);
void Interp_Car(Program* p, int pc);
int Interp_Car_helper(Program* p, char* lisp_str);
void Interp_Cdr(Program* p, int pc);
void Interp_Cdr_helper(char* lisp_str, Program* p);
void Interp_Boolfunc(Program* p, int i);
void Interp_Length(Program* p, int pc);
void Interp_Length_helper(char* lisp_str, Program* p);
void Interp_Plus(Program* p);
void Interp_Loop(Program* p, int loop_start);
void print_error(int i, Program* p);
void test(void);
