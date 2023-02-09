#include "specific.h"

void lisp_deep_copy(const lisp* l, lisp* l_cp);
void lisp_makestring(const lisp* l, char* str, bool is_car);
lisp* fromstring_helper(const char* str, int* x);
lisp* fromstr_sub_helper(const char* s, int* x, lisp* top);
bool string_checker(const char* str);
bool equal_brackets(const char* str);
bool empty_sublist(const char* str);
bool unexpected_character(const char* str);
int multi_digit(const char* str, int* x);
void times(lisp* l, atomtype* accum);
void atms(lisp* l, atomtype* accum);

lisp* lisp_atom(const atomtype a)
{
  lisp* l=(lisp*) ncalloc(1, sizeof(lisp));
  l->i=a;
  l->car=NULL;
  l->cdr=NULL;
  return l;
}


lisp* lisp_cons(const lisp* l1,  const lisp* l2)
{
  lisp* l=(lisp*) ncalloc(1, sizeof(lisp));
  l->car=(lisp*)l1;
  l->cdr=(lisp*)l2;
  return l;
}


lisp* lisp_car(const lisp* l)
{
  return l->car;
}


lisp* lisp_cdr(const lisp* l)
{
  return l->cdr;
}


atomtype lisp_getval(const lisp* l)
{
  return l->i;
}


bool lisp_isatomic(const lisp* l)
{
  if(l!=NULL && l->car==NULL && l->cdr==NULL){
    return true;
  }
  return false;
}


lisp* lisp_copy(const lisp* l)
{
  if(l==NULL){
    on_error("Error: Input to function lisp_copy is NULL");
  }
  lisp* l_cp=(lisp*) ncalloc(1, sizeof(lisp));
  lisp_deep_copy(l, l_cp);
  return l_cp;
}


void lisp_deep_copy(const lisp* l, lisp* l_cp)
{
  if(lisp_car(l)){
    lisp* car_cp=(lisp_atom(l->car->i));
    l_cp->car=car_cp;
    lisp_deep_copy(lisp_car(l), car_cp);
  }
  if(lisp_cdr(l)){
    lisp* cdr_cp=(lisp_atom(l->cdr->i));
    l_cp->cdr=cdr_cp;
    lisp_deep_copy(lisp_cdr(l), cdr_cp);
  }
}


int lisp_length(const lisp* l)
{
  int count=0;
  if(l==NULL || lisp_isatomic(l)){
    return 0;
  }
  if(lisp_cdr(l)!=NULL){
    count=lisp_length(lisp_cdr(l));
  }
  return count+1;
}


void lisp_tostring(const lisp* l, char* str)
{
  if(l==NULL){
    sprintf(str, "()");
    return;
  }
  memset(str, '\0', sizeof(char)*LISTSTRLEN);
  if(!lisp_isatomic(l)){
    lisp_makestring(l, str, true);
  }
  else{
    sprintf(str, "%i", lisp_getval(l));
  }
}


void lisp_makestring(const lisp* l, char* str, bool is_car)
{
  char temp_str[LISTSTRLEN]={'\0'};
  if(lisp_car(l) && is_car){
    strcat(str, "(");
  }
  if(lisp_isatomic(l)){
    sprintf(temp_str, "%i", lisp_getval(l));
    strcat(str, temp_str);
  }
  if(lisp_car(l)){
    lisp_makestring(lisp_car(l), str, true);
  }
  if(lisp_car(l) && lisp_cdr(l) && lisp_car(lisp_cdr(l))){
      strcat(str, " ");
  }
  if(lisp_cdr(l)){
    lisp_makestring(lisp_cdr(l), str, false);
  }
  else{
    if(!lisp_isatomic(l)){
      strcat(str, ")");
    }
  }
}


void lisp_free(lisp** l)
{
  if(l==NULL){
    return;
  }
  if(*l==NULL){
    return;
  }
  lisp_free(&(*l)->car);
  lisp_free(&(*l)->cdr);
  free(*l);
  *l=NULL;
}


lisp* lisp_fromstring(const char* str)
{
  int x=0;
  if (string_checker(str)==true){
    return fromstring_helper(str, &x);
  }
  return NULL;
}


char next_char(const char* str, int* x)
{
  char c=str[*x];
  *x+=1;
  return c;
}


lisp* fromstring_helper(const char* str, int* x)
{
  lisp* top=NULL;
  char c=next_char(str, x);
  if (c=='('){
    top=fromstr_sub_helper(str, x, top);
  }
  else if(isdigit(c)!=0 || c=='-'){
    *x-=1;
    return lisp_atom(multi_digit(str,x));
  }
  else{
    on_error("First character of the string not bracket or number.");
  }
  return top;
}


lisp* fromstr_sub_helper(const char* s, int* x, lisp* top)
{
  lisp* last=NULL;
  char c=next_char(s, x);
  while(c!=')'){
    if(c!=' '){
      *x-=1;
      lisp * l=lisp_cons(fromstring_helper(s, x), NULL);
      if (top==NULL){
        top=l;
      }
      if(last!=NULL){
        last->cdr=l;
      }
      last=l;
    }
    c=next_char(s,x);
  }
  return top;
}


bool string_checker(const char* str)
{
  if(equal_brackets(str)==false){
    on_error("The number of open and closed brackets do not match.");
  }
  if(empty_sublist(str)==true){
    on_error("LIST contains an empty sub list.");
  }
  if (unexpected_character(str)==true){
    on_error("LIST contains unexpected characters.");
  }
  return true;
}


bool equal_brackets(const char* str)
{
  int close=0;  int open=0;
  int x=0;
  while (str[x]!=EMPTY){
    char c=str[x];
    if(c=='('){
      open++;
    }
    if(c==')'){
      close++;
    }
    x++;
  }
  if(open!=close){
    return false;
  }
  return true;
}


bool empty_sublist(const char* str)
{
  int x=0;
  char temp1=EMPTY;  char temp2=EMPTY;
  while (str[x]!=EMPTY){
    if(x>1){
      temp1=str[x-1];
      temp2=str[x];
      if(temp1=='(' && temp2==')'){
        return true;
      }
    }
    x++;
  }
  return false;
}


int multi_digit(const char* str, int* x)
{
  int d = atoi(&str[*x]);
  if(str[*x]=='-' || str[*x]=='+'){
    (*x)+=1;
  }
  while (isdigit(str[*x])!=0){
    (*x)+=1;
  }
  return d;
}


bool unexpected_character(const char* str)
{
  int x=0;
  while (str[x]!=EMPTY){
    char c=str[x];
    if(c=='(' || c==')' || c==' ' || c=='-' || (c>='0' && c<='9')){
    }
    else{
      return true;
    }
    x++;
  }
  return false;
}


lisp* lisp_list(const int n, ...)
{
  if(n==0){
    on_error("Error: Zero Arguments Entered");
  }
  va_list argp;
  va_start(argp,n);

  lisp* list[MAXLIST];

  for(int i=0; i<n; i++){
    list[i]=va_arg(argp, lisp*);
  }

  lisp* l=lisp_cons(list[n-1], NULL);

  for(int i=n-2; i>=0; i--){
    l=lisp_cons(list[i], l);
  }
  va_end(argp);
  return l;
}


void lisp_reduce(void (*func)(lisp* l, atomtype* n), lisp* l, atomtype* acc)
{
  if(l==NULL){
    on_error("Error: Input to function lisp_reduce is NULL");
  }
  if(!lisp_isatomic(l) && lisp_car(l)){
    lisp_reduce(func, l->car, acc);
  }
  if(!lisp_isatomic(l) && lisp_cdr(l)){
    lisp_reduce(func, l->cdr, acc);
  }
  if(lisp_isatomic(l)){
    func(l, acc);
  }
}


void times(lisp* l, atomtype* accum)
{
   *accum = *accum * lisp_getval(l);
}


void atms(lisp* l, atomtype* accum)
{
   *accum = *accum + lisp_isatomic(l);
}


/*void test(void)
{
  char str[LISTSTRLEN];

  //lisp_atom tests
  lisp* l=lisp_atom(1);
  assert(l->i==1);
  assert(!l->car);
  assert(!l->cdr);

  //lisp_cons tests
  lisp*l1=(lisp_cons(lisp_atom(-1), lisp_cons(lisp_atom(2), NULL)));
  lisp*l2=(lisp_cons(l1, lisp_cons(lisp_atom(3), NULL)));
  assert(l1->car->i==-1);
  assert(l1->cdr->car->i==2);
  assert(!l1->cdr->cdr);
  assert(l2->car->car->i==-1);
  assert(l2->car->cdr->car->i==2);
  assert(l2->cdr->car->i==3);

  //lisp_car and lisp_cdr tests
  assert(!lisp_car(l));
  assert(!lisp_cdr(l));
  assert(lisp_car(l1)->i==-1);
  assert(lisp_car(l2)->car->i==-1);
  assert(lisp_cdr(l1)->car->i==2);
  assert(lisp_cdr(l2)->car->i==3);


  lisp* kmn=lisp_fromstring("(1(2 3))");
  lisp_tostring(kmn, str);
  printf("%s\n", str);
  printf("%i\n", (lisp_getval(lisp_car(kmn))));
  printf("%i\n", (lisp_getval(lisp_cdr(kmn)->car->car)));
  printf("%i\n", (lisp_getval(lisp_cdr(kmn)->car->cdr->car)));

  //lisp_getval tests
  assert(lisp_getval(lisp_car(l1))==-1);
  assert(lisp_getval(lisp_car(l2))==0);
  assert(lisp_getval(lisp_cdr(l1)->car)==2);
  assert(lisp_getval(lisp_cdr(l2)->car)==3);

  //lisp_isatomic tests
  assert(!lisp_isatomic(NULL));
  assert(lisp_isatomic(l));
  assert(!lisp_isatomic(l1));

  //lisp_copy tests
  lisp*l_cp1=(lisp_copy(l1));
  lisp_tostring(l_cp1, str);
  assert(strcmp(str, "(-1 2)")==0);
  lisp*l_cp2=(lisp_copy(l2));
  lisp_tostring(l_cp2, str);
  assert(strcmp(str, "((-1 2) 3)")==0);

  //lisp_deep_copy tests
  lisp*l3=lisp_cons(lisp_atom(1), lisp_cons(lisp_atom(2), NULL));
  lisp* l_cp3=(lisp*) ncalloc(1, sizeof(lisp));
  lisp_deep_copy(l3, l_cp3);
  lisp_tostring(l_cp3, str);
  assert(strcmp(str, "(1 2)")==0);

  //lisp_length tests
  assert(lisp_length(NULL)==0);
  assert(lisp_length(l)==0);
  assert(lisp_length(l1)==2);
  assert(lisp_length(l2)==2);

  //lisp_tostring tests
  lisp_tostring(l, str);
  assert(strcmp(str, "1")==0);
  lisp_tostring(l1, str);
  assert(strcmp(str, "(-1 2)")==0);
  lisp_tostring(l2, str);
  assert(strcmp(str, "((-1 2) 3)")==0);
  lisp_tostring(NULL, str);
  assert(strcmp(str, "()")==0);

  //lisp_free tests
  lisp_free(&l);
  assert(!l);
  lisp_free(&l2);
  assert(!l2);
  lisp_free(&l3);
  lisp_free(&l_cp1);
  lisp_free(&l_cp2);
  lisp_free(&l_cp3);

  //big lisp test - test all functions on a more complex lisp
  lisp* l4=lisp_cons(lisp_atom(6), NULL);
  lisp* l5=lisp_cons(lisp_cons(lisp_atom(4), lisp_cons(lisp_atom(5), NULL)), l4);
  lisp* l6=lisp_cons(lisp_atom(3), l5);
  lisp* l7=lisp_cons(lisp_atom(2), lisp_cons(l6, lisp_cons(lisp_atom(7), NULL)));
  lisp* l8=lisp_cons(lisp_cons(lisp_atom(0), lisp_cons(lisp_atom(1), NULL)), l7);
  assert(!lisp_isatomic(l8));
  lisp_tostring(l8, str);
  assert(strcmp(str, "((0 1) 2 (3 (4 5) 6) 7)")==0);
  lisp* l_cp8=lisp_copy(l8);
  lisp_tostring(l_cp8, str);
  assert(strcmp(str, "((0 1) 2 (3 (4 5) 6) 7)")==0);
  lisp_free(&l8);
  lisp_free(&l_cp8);

  //lisp_list tests
  lisp* g1= lisp_list(1, lisp_atom(3));
  assert(lisp_length(g1)==1);
  lisp_tostring(g1, str);
  assert(strcmp(str, "(3)")==0);
  lisp* g2= lisp_list(2, lisp_atom(3), lisp_atom(4));
  assert(lisp_length(g2)==2);
  lisp_tostring(g2, str);
  assert(strcmp(str, "(3 4)")==0);
  lisp_free(&g1);
  lisp_free(&g2);

  //from string test
  char inp[4][LISTSTRLEN] = {"()", "(1)", "(0 (1 -2) 3 4 50)", "((-1 2) (3 4) (5 (6 7)))"};
   for(int i=0; i<4; i++){
      lisp* f1 = lisp_fromstring(inp[i]);
      lisp_tostring(f1, str);
      assert(strcmp(str, inp[i])==0);
      lisp_free(&f1);
      assert(!f1);
   }

  //lisp_reduce tests
  lisp* l9=(lisp_cons(lisp_atom(2), lisp_cons(lisp_atom(3), NULL)));
  atomtype acc=1;
  lisp_reduce(times, l9, &acc);
  assert(acc==6);
  acc=0;
  lisp_reduce(atms, l9, &acc);
  assert(acc==2);
  lisp*l10=(lisp_cons(l9, lisp_cons(lisp_atom(-5), NULL)));
  acc=1;
  lisp_reduce(times, l10, &acc);
  assert(acc==-30);
  acc=0;
  lisp_reduce(atms, l10, &acc);
  assert(acc==3);
  lisp_free(&l10);
  assert(!(l10));
}*/
