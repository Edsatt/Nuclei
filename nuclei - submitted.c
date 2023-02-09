#include "nuclei.h"
#include "lisp.h"

int main(int argc, char* argv[])
{
  Program* p=ncalloc(1, sizeof(Program));
  initialise_program(p);
  FILE* file = fopen(argv[1], "r");
  if(argc<2){
    print_error(0,p);
  }
  if(!file){
    print_error(1,p);
  }
  test();
  FILE* pres_file = fopen(file_name(argv[1],".pres"), "w");
  FILE* ires_file = fopen(file_name(argv[1],".ires"), "w");

  point_tofiles(p, pres_file, ires_file);
  initialise_stack(p);
  interp_enable_check(p);

  char* str=ncalloc(MAX, sizeof(char));
  file_tostring(file, str);
  tokenise(p,str,0);
  Prog(p);
  fclose(file);
  fclose(pres_file);
  fclose(ires_file);
  free(p);
  free(str);
  return 0;
}

char* file_name(char* input, char* type)
{
  char* output=strtok(input, ".");
  strcat(output, type);
  return output;
}


void initialise_program(Program* p)
{
  p->flags.parse=1;
  p->flags.interp=0;
  p->flags.enable_error_messages=1;
  p->flags.parsing_failure=0;
  p->flags.interp_failure=0;
  p->pc=0;
  p->flags.output_str[0]='\0';
  p->flags.output_int[0]='\0';
  p->flags.output_bool=2;
  p->flags.intfunc=0;
  p->flags.conditional=0;
  for(int i=0; i<MAXNUMTOKENS; i++){
    p->tokens[i].type[0]='\0';
    p->tokens[i].value[0]='\0';
    p->tokens[i].has_data=0;
  }
  for(int i=0; i<ALPHA; i++){
    p->variables[i].var_set=0;
    p->variables[i].name[0]='\0';
    p->variables[i].value[0]='\0';
  }
}


void point_tofiles(Program* p, FILE* pres, FILE* ires)
{
  p->files.pres=pres;
  p->files.ires=ires;
}


void initialise_stack(Program* p)
{
  for(int r=0; r<MAXTOKENSIZE; r++){
    for(int c=0; c<STACKSIZE; c++){
      p->stack.data[c][r]='\0';
    }
  }
  p->stack.sp=0;
}


void interp_enable_check(Program* p)
{
  #ifdef INTERP
    p->flags.interp=1;
    return;
  #else
    p->flags.interp=0;
    return;
  #endif
}


void file_tostring(FILE* file, char* str)
{
  char temp_str[MAX]={'\0'};
  char file_str[MAX]={'\0'};
  while(fgets(temp_str,MAX,file)!='\0'){
    if(temp_str[0]!='#'){
      strcat(file_str, temp_str);
      file_str[strlen(file_str)-1]='\0';
    }
  }
  string_clean(str, file_str);
}

//_____________________Tokeniser_______________________
void string_clean(char* str, char* temp_str)//Removes spaces unless spaces are within quotes
{
  int length=strlen(temp_str);
  int x=0;
  for(int i=0; i<length; i++){
    if(temp_str[i]=='\"' || temp_str[i]=='\''){
      char c=temp_str[i];
      do{
        str[x]=temp_str[i];
        x++,i++;
      }
      while(temp_str[i]!=c);
    }
    if(temp_str[i]!=' '){
      str[x]=temp_str[i];
      x++;
    }
  }
}


void tokenise(Program* p, char* str, int i)
{
  int length=strlen(str);
  if(i<length){
    if(!char_sorter(str,i,p)){
      print_error(2,p);
    }
  }
}


bool char_sorter(char* str, int i, Program* p)
{
  if(str[i]=='(' || str[i]==')'){
    tokenise_singlechar(str, i, p);
    return true;
  }
  if(str[i]=='\''){
    tokenise_stringorliteral(str, "LITERAL", i, p);
    return true;
  }
  if(str[i]=='\"'){
    tokenise_stringorliteral(str, "STRING", i, p);
    return true;
  }
  if(str[i]>='A' && str[i]<='Z'){
    if(is_keyword(str, i, p)){
      return true;
    }
    else{
      tokenise_singlechar(str, i, p);
      return true;
    }
  }
  return false;
}


bool is_keyword(char* str, int i, Program* p)
{
  switch(str[i])
  {
    case 'C':
      if(tokenise_keyword("LISTFUNC", str, "CAR", i, p)){
        tokenise(p, str, i+3);
        return true;
      }
      if(tokenise_keyword("LISTFUNC", str, "CDR", i, p)){
        tokenise(p, str, i+3);
        return true;
      }
      if(tokenise_keyword("LISTFUNC", str, "CONS", i, p)){
        tokenise(p, str, i+4);
        return true;
      }
      break;
    case 'E':
      if(tokenise_keyword("BOOLFUNC", str, "EQUAL", i, p)){
        tokenise(p, str, i+5);
        return true;
      }
      break;
    case 'G':
      if(tokenise_keyword("BOOLFUNC", str, "GREATER", i, p)){
        tokenise(p, str, i+7);
        return true;
      }
      break;
    case 'I':
      if(tokenise_keyword("IF", str, "IF", i, p)){
        tokenise(p, str, i+2);
        return true;
      }
      break;
    case 'L':
      if(tokenise_keyword("INTFUNC", str, "LENGTH", i, p)){
        tokenise(p, str, i+6);
        return true;
      }
      if(tokenise_keyword("BOOLFUNC", str, "LESS", i, p)){
        tokenise(p, str, i+4);
        return true;
      }
      break;
    case 'N':
      if(tokenise_keyword("NIL", str, "NIL", i, p)){
        tokenise(p, str, i+3);
        return true;
      }
      break;
    case 'P':
      if(tokenise_keyword("INTFUNC", str, "PLUS", i, p)){
        tokenise(p, str, i+4);
        return true;
      }
      if(tokenise_keyword("IOFUNC", str, "PRINT", i, p)){
        tokenise(p, str, i+5);
        return true;
      }
      break;
    case 'S':
      if(tokenise_keyword("IOFUNC", str, "SET", i, p)){
        tokenise(p, str, i+3);
        return true;
      }
      break;
    case 'W':
      if(tokenise_keyword("LOOP", str, "WHILE", i, p)){
        tokenise(p, str, i+5);
        return true;
      }
      break;
  }
  return false;
}


void tokenise_stringorliteral(char* str, char* type, int i, Program* p)
{
  char value[MAXTOKENSIZE]={'\0'};
  int y=0;
  for(int x=i+1; x<(i+MAXTOKENSIZE); x++){
    value[y]=str[x];
    if(str[x+1]==str[i]){
      store_token(type, value, p);
      tokenise(p, str, i+y+3);
      return;
    }
    y++;
  }
}


void tokenise_singlechar(char* str, int i, Program* p)
{
  char value[MAXTOKENSIZE]={'\0'};
  value[0]=str[i];
  if(str[i]=='('){
    store_token("LBRACE", value, p);
  }
  if(str[i]==')'){
    store_token("RBRACE", value, p);
  }
  if(str[i]>='A' && str[i]<='Z'){
    store_token("VAR", value, p);
  }
  tokenise(p, str, i+1);
}


bool tokenise_keyword(char* type, char* str, char* cmp_str, int i, Program* p)
{
  int length=strlen(cmp_str);
  int y=0;
  for(int x=i; x<(i+length); x++){
    if(cmp_str[y]!=str[x]){
      return false;
    }
    y++;
  }
  store_token(type, cmp_str, p);
  return true;
}


bool store_token(char* type, char* value, Program* p)
{
  for(int i=0; i<MAXNUMTOKENS; i++){
    if(p->tokens[i].has_data==0){
      strcpy(p->tokens[i].type,type);
      strcpy(p->tokens[i].value,value);
      p->tokens[i].has_data=1;
      return true;
    }
  }
  return false;
}

//______________________Parser________________________
bool Prog(Program* p)
{
  if(!(strsame(p->tokens[p->pc].type, "LBRACE"))){
    p->flags.parsing_failure=1;
    print_error(3,p);
  }
  p->pc++;
  Instrcts(p);
  if(p->flags.parse){
    fprintf(p->files.pres,"Parsed OK\n");
  }
  if(!p->flags.parsing_failure){
    return true;
  }
  return false;
}


void Instrcts(Program* p)
{
  if(strsame(p->tokens[p->pc].type, "RBRACE")){
    return;
  }
  Instrct(p);
}


void Instrct(Program* p)
{
  if(!(strsame(p->tokens[p->pc].type, "LBRACE"))){
    p->flags.parsing_failure=1;
    print_error(3,p);
    return;
  }
  p->pc++;
  Func(p);
  p->pc++;
  if(!(strsame(p->tokens[p->pc].type, "RBRACE"))){
    p->flags.parsing_failure=1;
    print_error(4,p);
  }
  p->pc++;
  Instrcts(p);
}


void Func(Program* p)
{
  if(Retfunc(p)){
    return;
  }
  if(Iofunc(p)){
    return;
  }
  if(If_statement(p)){
    return;
  }
  if(Loop(p)){
    return;
  }
  print_error(5,p);
}


bool Retfunc(Program* p)
{
  if(strsame(p->tokens[p->pc].type, "LISTFUNC")){
    Listfunc(p);
    return true;
  }
  if(strsame(p->tokens[p->pc].type, "INTFUNC")){
    Intfunc(p);
    return true;
  }
  if(strsame(p->tokens[p->pc].type, "BOOLFUNC")){
    Boolfunc(p);
    return true;
  }
  return false;
}


bool Iofunc(Program* p)
{
  if(strsame(p->tokens[p->pc].value, "SET")){
    Set(p);
    return true;
  }
  if(strsame(p->tokens[p->pc].value, "PRINT")){
    Print(p);
    return true;
  }
  return false;
}


bool If_statement(Program* p)
{
  if(!(strsame(p->tokens[p->pc].value, "IF"))){
    return false;
  }
  p->pc++;
  if(!(strsame(p->tokens[p->pc].type, "LBRACE"))){
    return false;
  }
  p->pc++;
  Boolfunc(p);
  p->pc++;
  if(!(strsame(p->tokens[p->pc].type, "RBRACE"))){
    return false;
  }
  p->flags.conditional=1;
  p->pc++;
  if(!(strsame(p->tokens[p->pc].type, "LBRACE"))){
    return false;
  }
  p->pc++;
  Instrcts(p);
  p->pc++;
  if(!(strsame(p->tokens[p->pc].type, "LBRACE"))){
    return false;
  }
  p->pc++;
  flip_output_bool(p);
  Instrcts(p);
  p->flags.conditional=0;
  return true;
}


void flip_output_bool(Program* p)
{
  if(p->flags.output_bool){
    p->flags.output_bool=0;
    return;
  }
  p->flags.output_bool=1;
}


bool Loop(Program* p)
{
  if(!(strsame(p->tokens[p->pc].value, "WHILE"))){
    return false;
  }
  p->pc++;
  int loop_start=p->pc;
  if(!(strsame(p->tokens[p->pc].type, "LBRACE"))){
    return false;
  }
  p->pc++;
  Boolfunc(p);
  p->pc++;
  if(!(strsame(p->tokens[p->pc].type, "RBRACE"))){
    return false;
  }
  p->pc++;
  if(!(strsame(p->tokens[p->pc].type, "LBRACE"))){
    return false;
  }
  p->pc++;
  Instrcts(p);
  Interp_Loop(p, loop_start);
  return true;
}


void Intfunc(Program* p)
{
  if(strsame(p->tokens[p->pc].value, "LENGTH")){
    p->pc++;
    int pc_length=p->pc;
    p->flags.intfunc=1;
    List(p);
    Interp_Length(p, pc_length);
    p->flags.intfunc=0;
    return;
  }
  if(strsame(p->tokens[p->pc].value, "PLUS")){
    for(int i=0; i<2; i++){
      p->pc++;
      List(p);
      p->flags.intfunc=1;
      Push(p, p->pc);
    }
    Interp_Plus(p);
    p->flags.intfunc=0;
    return;
  }
  p->flags.parsing_failure=1;
  print_error(6,p);
}


void Boolfunc(Program* p)
{
  if(strsame(p->tokens[p->pc].type, "BOOLFUNC")){
    int pc_save=p->pc;
    for(int i=0; i<2; i++){
      p->pc++;
      List(p);
      p->flags.intfunc=1;
      Push(p, p->pc);
    }
    if(strsame(p->tokens[pc_save].value, "LESS")){
      Interp_Boolfunc(p, 1);
    }
    if(strsame(p->tokens[pc_save].value, "GREATER")){
      Interp_Boolfunc(p, 2);
    }
    if(strsame(p->tokens[pc_save].value, "EQUAL")){
      Interp_Boolfunc(p, 3);
    }
    p->flags.intfunc=0;
    return;
  }
  p->flags.parsing_failure=1;
  print_error(7,p);
}


void Listfunc(Program* p)
{
  if(strsame(p->tokens[p->pc].value, "CAR")){
    p->pc++;
    int pc_car=p->pc;
    List(p);
    Interp_Car(p,pc_car);
    return;
  }
  if(strsame(p->tokens[p->pc].value, "CDR")) {
    p->pc++;
    int pc_cdr=p->pc;
    List(p);
    Interp_Cdr(p,pc_cdr);
    return;
  }
  if(strsame(p->tokens[p->pc].value, "CONS")){
    for(int i=0; i<2; i++){
      p->pc++;
      List(p);
      Push(p, p->pc);
    }
    Interp_Cons(p);
    return;
  }
}


void List(Program* p)
{
  if(strsame(p->tokens[p->pc].type, "VAR")){
    return;
  }
  if(strsame(p->tokens[p->pc].type, "LITERAL")){
    return;
  }
  if(strsame(p->tokens[p->pc].type, "NIL")){
    return;
  }
  if(strsame(p->tokens[p->pc].type, "LBRACE")){
    p->pc++;
    Retfunc(p);
    p->pc++;
    if(strsame(p->tokens[p->pc].type, "RBRACE")){
    return;
    }
  }
  p->flags.parsing_failure=1;
  print_error(8,p);
}


void Set(Program* p)
{
  p->pc++;
  int pc_save=p->pc;
  if(strsame(p->tokens[p->pc].type, "VAR")){
    p->pc++;
    List(p);
    Interp_Set(p, pc_save);
    return;
  }
  p->flags.parsing_failure=1;
  print_error(9,p);
}


void Print(Program* p)
{

  p->pc++;
  int pc_save=p->pc;
  if(strsame(p->tokens[p->pc].type, "STRING")){
    Interp_Print(p, pc_save);
    return;
  }
  List(p);
  Interp_Print(p, pc_save);
  return;
}

//______________________Interpreter________________________
void Interp_Print(Program* p, int pc)
{
  if(!p->flags.interp){
    return;
  }
  if(p->flags.conditional && !p->flags.output_bool){
    return;
  }
  if(strsame(p->tokens[p->pc].type,"STRING")){
    fprintf(p->files.ires,"%s\n",p->tokens[pc].value);
  }
  if(strsame(p->tokens[pc].type,"VAR")){
    fprintf(p->files.ires,"%s\n", get_variable(p, pc));
  }
  if(strsame(p->tokens[pc+1].type,"LISTFUNC")){
    fprintf(p->files.ires,"%s\n", p->flags.output_str);
    p->flags.output_str[0]='\0';
  }
  if(strsame(p->tokens[pc+1].type,"INTFUNC")){
    fprintf(p->files.ires,"%s\n", p->flags.output_int);
    p->flags.output_int[0]='\0';
  }
}


void Interp_Set(Program* p, int pc)
{
  if(!p->flags.interp){
    return;
  }
  if(p->flags.conditional && !p->flags.output_bool){
    return;
  }
  if(strsame(p->tokens[pc+1].type, "VAR")){
    store_variable((p->tokens[pc].value),(get_variable(p, pc+1)),p);
  }
  if(strsame(p->tokens[pc+2].type, "INTFUNC")){
    store_variable((p->tokens[pc].value),p->flags.output_int,p);
    p->flags.output_str[0]='\0';
  }
  if(strsame(p->tokens[pc+2].type, "BOOLFUNC")){
    store_variable(p->tokens[pc].value,bool_to_string(p),p);
    p->flags.output_str[0]='\0';
  }
  if(strsame(p->tokens[pc+2].type, "LISTFUNC")){
    store_variable((p->tokens[pc].value),p->flags.output_str,p);
    p->flags.output_str[0]='\0';
  }
  if(strsame(p->tokens[pc+1].type, "LITERAL") || strsame(p->tokens[pc+1].type, "NIL")){
    store_variable((p->tokens[pc].value),(p->tokens[pc+1].value),p);
  }
}


char* bool_to_string(Program* p)
{
  if(p->flags.output_bool){
    return "1";
  }
  return "0";
}


void store_variable(char* name, char* value, Program* p)
{
  int index=(name[0]-ASCII_A);
  strcpy(p->variables[index].name,name);
  strcpy(p->variables[index].value,value);
  p->variables[index].var_set=1;
}


char* get_variable(Program* p, int pc)
{
  int index=(p->tokens[pc].value[0]-ASCII_A);
  if(!p->variables[index].var_set){
    p->flags.interp_failure=1;
    print_error(10, p);
    return "\0";
  }
  return p->variables[index].value;
}


void Push(Program* p, int pc)
{
  if(!p->flags.interp){
    return;
  }
  if(strsame(p->tokens[pc].type,"VAR")){
    strcpy(p->stack.data[p->stack.sp], get_variable(p, pc));
    p->stack.sp++;
  }
  if((strsame(p->tokens[pc].type, "NIL")) || (strsame(p->tokens[pc].type, "LITERAL"))){
    strcpy(p->stack.data[p->stack.sp], p->tokens[pc].value);
    p->stack.sp++;
  }
  if(strsame(p->tokens[pc].type,"RBRACE") && p->flags.intfunc==1){
    strcpy(p->stack.data[p->stack.sp], p->flags.output_int);
    p->stack.sp++;
  }
  if(strsame(p->tokens[pc].type,"LISTFUNC")){
    strcpy(p->stack.data[p->stack.sp], p->flags.output_str);
    p->stack.sp++;
  }
  return;
}


char* Pop(Program* p)
{
  p->stack.sp--;
  return p->stack.data[p->stack.sp];
}


void Interp_Cons(Program* p)
{
  if(!p->flags.interp){
    return;
  }
  if(p->flags.conditional && !p->flags.output_bool){
    return;
  }
  if((strsame(p->tokens[p->pc].value, "NIL")) || (strsame(p->tokens[p->pc].type, "VAR"))  || (strsame(p->tokens[p->pc].type, "LITERAL"))){
    lisp* l=NULL;
    l=Interp_Cons_helper(l,p);
    lisp_tostring(l, p->flags.output_str);
    lisp_free(&l);
  }
}


lisp* Interp_Cons_helper(lisp* l, Program* p)
{
  char end_str[LISTSTRLEN]={'\0'};
  strcpy(end_str, Pop(p));
  if(strsame(end_str, "NIL")){
    l=NULL;
  }
  else if(!(strsame(end_str, "NIL"))){
    l=lisp_fromstring(end_str);
  }
  while(p->stack.sp!=0){
    l=lisp_cons(lisp_fromstring(Pop(p)), l);
  }
  return l;
}


void Interp_Car(Program* p, int pc)
{
  if(!p->flags.interp){
    return;
  }
  if(p->flags.conditional && !p->flags.output_bool){
    return;
  }
  int car;
  if(strsame(p->tokens[pc].type,"VAR")){
    car=Interp_Car_helper(p, get_variable(p, pc));
    sprintf(p->flags.output_int, "%d", car);
    return;
  }
  if(strsame(p->tokens[pc].type,"LITERAL")){
    car=Interp_Car_helper(p, p->tokens[pc].value);
    sprintf(p->flags.output_int, "%d", car);
    return;
  }
  car=Interp_Car_helper(p, p->flags.output_str);
  sprintf(p->flags.output_int, "%d", car);
}


int Interp_Car_helper(Program* p, char* lisp_str)
{
  lisp* temp_lisp=lisp_fromstring(lisp_str);
  int car=lisp_getval(lisp_car(temp_lisp));
  lisp_tostring(lisp_car(temp_lisp), p->flags.output_str);
  lisp_free(&temp_lisp);
  return car;
}


void Interp_Cdr(Program* p, int pc)
{
  if(!p->flags.interp){
    return;
  }
  if(p->flags.conditional && !p->flags.output_bool){
    return;
  }
  if(strsame(p->tokens[pc].type,"VAR")){
    Interp_Cdr_helper(get_variable(p, pc),p);
    return;
  }
  if(strsame(p->tokens[pc].type,"LITERAL")){
    Interp_Cdr_helper(p->tokens[pc].value,p);
    return;
  }
  Interp_Cdr_helper(p->flags.output_str,p);
}


void Interp_Cdr_helper(char* lisp_str, Program* p)
{
  lisp* temp_lisp=lisp_fromstring(lisp_str);
  lisp* cdr=lisp_cdr(temp_lisp);
  lisp_tostring(cdr, p->flags.output_str);
  lisp_free(&temp_lisp);
}


void Interp_Boolfunc(Program* p, int i)
{
  if(!p->flags.interp){
    return;
  }
  if(p->flags.conditional && !p->flags.output_bool){
    return;
  }
  int i2=atoi(Pop(p));
  int i1=atoi(Pop(p));
  switch (i)
  {
    case 1://Less than
      if(i1<i2){
        p->flags.output_bool=1;
      }
      else if(i1>=i2){
        p->flags.output_bool=0;
      }
      break;
    case 2://Greater than
      if(i1>i2){
        p->flags.output_bool=1;
      }
      else if(i1<=i2){
        p->flags.output_bool=0;
      }
      break;
    case 3://Equal
      if(i1==i2){
        p->flags.output_bool=1;
      }
      else if(i1!=i2){
        p->flags.output_bool=0;
      }
      break;
  }
}


void Interp_Length(Program* p, int pc)
{
  if(!p->flags.interp){
    return;
  }
  if(p->flags.conditional && !p->flags.output_bool){
    return;
  }
  if(strsame(p->tokens[pc].type,"VAR")){
    Interp_Length_helper(get_variable(p, pc),p);
    return;
  }
  if(strsame(p->tokens[pc].type,"LITERAL")){
    Interp_Length_helper(p->tokens[pc].value,p);
    return;
  }
  if(strsame(p->tokens[pc].type,"NIL")){
    Interp_Length_helper(p->tokens[pc].value,p);
    return;
  }
  Interp_Length_helper(p->flags.output_str,p);
}


void Interp_Length_helper(char* lisp_str, Program* p)
{
  if(strsame(lisp_str,"NIL")){
    sprintf(p->flags.output_int, "%d", 0);
    return;
  }
  lisp* temp_lisp=lisp_fromstring(lisp_str);
  int length=lisp_length(temp_lisp);
  p->flags.output_str[0]='\0';
  sprintf(p->flags.output_int, "%d", length);
  lisp_free(&temp_lisp);
}


void Interp_Plus(Program* p)
{
  if(!p->flags.interp){
    return;
  }
  if(p->flags.conditional && !p->flags.output_bool){
    return;
  }
  int sum=(atoi(Pop(p))+atoi(Pop(p)));
  sprintf(p->flags.output_int, "%d", sum);
}


void Interp_Loop(Program* p, int loop_start)
{
  if(!p->flags.interp){
    return;
  }
  int loop_end=p->pc;
  p->pc=loop_start;
  while(p->flags.output_bool){
    p->pc++;
    Boolfunc(p);
    if(!p->flags.output_bool){
      p->pc=loop_end;
      return;
    }
    p->pc=p->pc+3;
    Instrcts(p);
    p->pc=loop_start;
  }
  p->pc=loop_end;
  return;
}


void print_error(int i, Program* p)
{
  if(!p->flags.enable_error_messages){
    return;
  }
  switch(i)
  {
    case 0:
      fprintf(stderr, "Expecting file?\n");
      exit(EXIT_FAILURE);
      break;
    case 1:
      fprintf(stderr, "File could not be opened\n");
      exit(EXIT_FAILURE);
      break;
    case 2:
      fprintf(stderr, "Invalid character\n");
      exit(EXIT_FAILURE);
      break;
    case 3:
      fprintf(stderr, "No front brace\n");
      exit(EXIT_FAILURE);
      break;
    case 4:
      fprintf(stderr, "Expecting closing brace?\n");
      exit(EXIT_FAILURE);
      break;
    case 5:
      fprintf(stderr, "Expecting function\n");
      exit(EXIT_FAILURE);
      break;
    case 6:
      fprintf(stderr, "Expecting intfunc?\n");
      exit(EXIT_FAILURE);
      break;
    case 7:
      fprintf(stderr, "Expecting boolfunc\n");
      exit(EXIT_FAILURE);
      break;
    case 8:
      fprintf(stderr, "Expecting valid list?\n");
      exit(EXIT_FAILURE);
      break;
    case 9:
      fprintf(stderr, "Expecting variable\n");
      exit(EXIT_FAILURE);
      break;
    case 10:
      fprintf(stderr, "Use of unset variable\n");
      exit(EXIT_FAILURE);
      break;
  }
}


void test(void)
{
  //________________tokeniser tests________________________________________
  Program* p=ncalloc(1, sizeof(Program));
  char* str=ncalloc(MAX, sizeof(char));

  //initialise_program tests
  initialise_program(p);
  assert(p->tokens[0].has_data==0);
  assert(strsame(p->tokens[0].value, "\0"));
  assert(strsame(p->tokens[0].type, "\0"));
  assert(p->tokens[199].has_data==0);
  assert(strsame(p->tokens[199].value, "\0"));
  assert(strsame(p->tokens[199].type, "\0"));

  //disable error printing for testing purposes
  p->flags.parse=0;
  p->flags.enable_error_messages=0;

  //string_clean tests
  string_clean(str, "TEST STRING ONE");
  assert(!(strsame(str, "TEST STRING ONE")));
  string_clean(str, "TEST STRING ONE");
  assert(strsame(str, "TESTSTRINGONE"));
  string_clean(str, "TEST STRING \"this is a quote\"");
  assert(strsame(str, "TESTSTRING\"this is a quote\""));
  free(str);

  //char_sorter tests
  char* str_1={"(CARcar1\")\0"};
  assert(char_sorter(str_1,0,p));
  assert(char_sorter(str_1,3,p));
  assert(!char_sorter(str_1,4,p));
  assert(!char_sorter(str_1,7,p));

  //is_keyword tests
  char* str_2={"CARCDRCONSEQUALGREATERIFLENGTHLESSNILPLUSPRINTSETWHILEcar"};
  assert(is_keyword(str_2, 0, p));
  assert(is_keyword(str_2, 3, p));
  assert(is_keyword(str_2, 6, p));
  assert(is_keyword(str_2, 10, p));
  assert(is_keyword(str_2, 15, p));
  assert(is_keyword(str_2, 22, p));
  assert(is_keyword(str_2, 24, p));
  assert(is_keyword(str_2, 30, p));
  assert(is_keyword(str_2, 34, p));
  assert(is_keyword(str_2, 37, p));
  assert(is_keyword(str_2, 41, p));
  assert(is_keyword(str_2, 46, p));
  assert(is_keyword(str_2, 49, p));
  assert(!is_keyword(str_2, 54, p));

  //store_token tests
  initialise_program(p);
  p->flags.parse=0;
  p->flags.enable_error_messages=0;
  char* str_3={"(SETA'1')\0"};
  tokenise(p, str_3, 0);
  assert(strsame(p->tokens[0].type,"LBRACE"));
  assert(strsame(p->tokens[0].value,"("));
  assert(p->tokens[0].has_data);
  assert(strsame(p->tokens[1].type,"IOFUNC"));
  assert(strsame(p->tokens[1].value,"SET"));
  assert(strsame(p->tokens[2].type,"VAR"));
  assert(strsame(p->tokens[2].value,"A"));
  assert(strsame(p->tokens[3].type,"LITERAL"));
  assert(strsame(p->tokens[3].value,"1"));
  assert(strsame(p->tokens[4].type,"RBRACE"));
  assert(strsame(p->tokens[4].value,")"));
  assert(!p->tokens[5].has_data);

  //________________Parser tests________________________________________
  initialise_program(p);
  p->flags.parse=0;
  p->flags.enable_error_messages=0;
  char* str_4={"()\0"};
  tokenise(p,str_4,0);
  assert(Prog(p));

  //Listfunc tests
  initialise_program(p);
  p->flags.parse=0;
  p->flags.enable_error_messages=0;
  char* str_5={"(CAR)\0"};
  tokenise(p,str_5,0);
  assert(!Prog(p));

  initialise_program(p);
  p->flags.parse=0;
  p->flags.enable_error_messages=0;
  char* str_6={"(CARA)\0"};
  tokenise(p,str_6,0);
  assert(!Prog(p));

  initialise_program(p);
  p->flags.parse=0;
  p->flags.enable_error_messages=0;
  char* str_7={"((CARA))\0"};
  tokenise(p,str_7,0);
  assert(Prog(p));

  initialise_program(p);
  p->flags.parse=0;
  p->flags.enable_error_messages=0;
  char* str_8={"((CONSA))\0"};
  tokenise(p,str_8,0);
  assert(!Prog(p));

  initialise_program(p);
  p->flags.parse=0;
  p->flags.enable_error_messages=0;
  char* str_9={"((CONSAB))\0"};
  tokenise(p,str_9,0);
  assert(Prog(p));

  initialise_program(p);
  p->flags.parse=0;
  p->flags.enable_error_messages=0;
  char* str_10={"((CONS(CAR'1')(CARNIL)))\0"};
  tokenise(p,str_10,0);
  assert(Prog(p));

  //Intfunc tests
  initialise_program(p);
  p->flags.parse=0;
  p->flags.enable_error_messages=0;
  char* str_11={"((PLUS(CARA)B)(LENGTH(CONSAB)))\0"};
  tokenise(p,str_11,0);
  assert(Prog(p));

  initialise_program(p);
  p->flags.parse=0;
  p->flags.enable_error_messages=0;
  char* str_12={"((PLUS(CARA))(LENGTH(CONSAB)))\0"};
  tokenise(p,str_12,0);
  assert(!Prog(p));

  //Boolfunc test
  initialise_program(p);
  p->flags.parse=0;
  p->flags.enable_error_messages=0;
  char* str_13={"((LESSAB)(GREATERAB)(EQUALAB))\0"};
  tokenise(p,str_13,0);
  assert(Prog(p));

  //Iofunc tests
  initialise_program(p);
  p->flags.parse=0;
  p->flags.enable_error_messages=0;
  char* str_14={"((SETA(CARB)))\0"};
  tokenise(p,str_14,0);
  assert(Prog(p));

  initialise_program(p);
  p->flags.parse=0;
  p->flags.enable_error_messages=0;
  char* str_15={"((SET(CARB)A))\0"};
  tokenise(p,str_15,0);
  assert(!Prog(p));

  initialise_program(p);
  p->flags.parse=0;
  p->flags.enable_error_messages=0;
  char* str_16={"((PRINTA)(PRINT\"string\"))\0"};
  tokenise(p,str_16,0);
  assert(Prog(p));

  //If statement tests
  initialise_program(p);
  p->flags.parse=0;
  p->flags.enable_error_messages=0;
  char* str_17={"((IF(EQUAL(CARL)'2')((PRINT\"L1hasCar2\"))((PRINT\"FAILURE\"))))\0"};
  tokenise(p,str_17,0);
  assert(Prog(p));

  initialise_program(p);
  p->flags.parse=0;
  p->flags.enable_error_messages=0;
  char* str_18={"((IF(EQUAL'1''1'((PRINT\"YES\"))((GARBAGE))))\0"};
  tokenise(p,str_18,0);
  assert(!Prog(p));

  //Loop test
  initialise_program(p);
  p->flags.parse=0;
  p->flags.enable_error_messages=0;
  char* str_19={"((WHILE(LESS'1''2')((PRINT\"LOOPFOREVER\"))))\0"};
  tokenise(p,str_19,0);
  assert(Prog(p));

  //________________Interpreter tests________________________________________

  //Push tests
  initialise_program(p);
  initialise_stack(p);
  p->flags.parse=0;
  p->flags.interp=1;
  p->flags.enable_error_messages=0;
  char* str_20={"((SETA'1')(SETB'(2 1)'))\0"};
  tokenise(p,str_20,0);
  assert(Prog(p));
  Push(p, 4);
  assert(strsame(p->stack.data[0], "1"));
  Push(p, 9);
  assert(strsame(p->stack.data[1], "(2 1)"));
  assert(p->stack.sp==2);
  assert(strsame(p->stack.data[3], "\0"));
  assert(p->stack.sp==2);

  //Pop tests
  assert(strsame(Pop(p), "(2 1)"));
  assert(strsame(Pop(p), "1"));

  //Interp_Set tests
  initialise_program(p);
  initialise_stack(p);
  p->flags.parse=0;
  p->flags.interp=1;
  p->flags.enable_error_messages=0;
  char* str_21={"((SETA'1')(SETB'(2 1)')(SETC'1')(SETCB)(SETDE))\0"};
  tokenise(p,str_21,0);
  assert(Prog(p));
  assert(strsame(p->variables[0].name, "A"));
  assert(strsame(p->variables[0].value, "1"));
  assert(strsame(p->variables[1].name, "B"));
  assert(strsame(p->variables[1].value, "(2 1)"));
  assert(strsame(p->variables[2].name, "C"));
  assert(strsame(p->variables[2].value, "(2 1)"));
  assert(p->flags.interp_failure);//Interp fails at final SET instruction as var E has not been set

  //Interp_Cons tests
  initialise_program(p);
  initialise_stack(p);
  p->flags.parse=0;
  p->flags.interp=1;
  p->flags.enable_error_messages=0;
  char* str_22={"((CONS'1'(CONS'2'NIL)))\0"};
  tokenise(p,str_22,0);
  assert(Prog(p));
  assert(strsame(p->flags.output_str,"(1 2)"));

  initialise_program(p);
  initialise_stack(p);
  p->flags.parse=0;
  p->flags.interp=1;
  p->flags.enable_error_messages=0;
  char* str_23={"((SETA'(1 2)')(SETB'(3)')(CONSAB))\0"};
  tokenise(p,str_23,0);
  assert(Prog(p));
  assert(strsame(p->flags.output_str,"((1 2) 3)"));

  //Interp_CAR and Interp_CDR tests
  initialise_program(p);
  initialise_stack(p);
  p->flags.parse=0;
  p->flags.interp=1;
  p->flags.enable_error_messages=0;
  char* str_24={"((SETA'(1 (2 3))')(CARA)(CDRA))\0"};
  tokenise(p,str_24,0);
  assert(Prog(p));
  assert(strsame(p->flags.output_int,"1"));
  assert(strsame(p->flags.output_str,"((2 3))"));

  //Interp_Boolfunc tests
  initialise_program(p);
  initialise_stack(p);
  p->flags.parse=0;
  p->flags.interp=1;
  p->flags.enable_error_messages=0;
  char* str_25={"((LESS'1''2'))\0"};
  tokenise(p,str_25,0);
  assert(Prog(p));
  assert(p->flags.output_bool);

  initialise_program(p);
  initialise_stack(p);
  p->flags.parse=0;
  p->flags.interp=1;
  p->flags.enable_error_messages=0;
  char* str_26={"((LESS'2''2'))\0"};
  tokenise(p,str_26,0);
  assert(Prog(p));
  assert(!p->flags.output_bool);

  initialise_program(p);
  initialise_stack(p);
  p->flags.parse=0;
  p->flags.interp=1;
  p->flags.enable_error_messages=0;
  char* str_27={"((GREATER'2''1'))\0"};
  tokenise(p,str_27,0);
  assert(Prog(p));
  assert(p->flags.output_bool);

  initialise_program(p);
  initialise_stack(p);
  p->flags.parse=0;
  p->flags.interp=1;
  p->flags.enable_error_messages=0;
  char* str_28={"((GREATER'2''2'))\0"};
  tokenise(p,str_28,0);
  assert(Prog(p));
  assert(!p->flags.output_bool);

  initialise_program(p);
  initialise_stack(p);
  p->flags.parse=0;
  p->flags.interp=1;
  p->flags.enable_error_messages=0;
  char* str_29={"((EQUAL'2''2'))\0"};
  tokenise(p,str_29,0);
  assert(Prog(p));
  assert(p->flags.output_bool);

  initialise_program(p);
  initialise_stack(p);
  p->flags.parse=0;
  p->flags.interp=1;
  p->flags.enable_error_messages=0;
  char* str_30={"((EQUAL'1''2'))\0"};
  tokenise(p,str_30,0);
  assert(Prog(p));
  assert(!p->flags.output_bool);

  //Interp_Length tests
  initialise_program(p);
  initialise_stack(p);
  p->flags.parse=0;
  p->flags.interp=1;
  p->flags.enable_error_messages=0;
  char* str_31={"((LENGTH'(1 2)'))\0"};
  tokenise(p,str_31,0);
  assert(Prog(p));
  assert(strsame(p->flags.output_int, "2"));

  initialise_program(p);
  initialise_stack(p);
  p->flags.parse=0;
  p->flags.interp=1;
  p->flags.enable_error_messages=0;
  char* str_32={"((LENGTH'(1 (2 3))'))\0"};
  tokenise(p,str_32,0);
  assert(Prog(p));
  assert(strsame(p->flags.output_int, "2"));

  //Interp_Plus tests
  initialise_program(p);
  initialise_stack(p);
  p->flags.parse=0;
  p->flags.interp=1;
  p->flags.enable_error_messages=0;
  char* str_33={"((PLUS'1''2'))\0"};
  tokenise(p,str_33,0);
  assert(Prog(p));
  assert(strsame(p->flags.output_int, "3"));

  initialise_program(p);
  initialise_stack(p);
  p->flags.parse=0;
  p->flags.interp=1;
  p->flags.enable_error_messages=0;
  char* str_34={"((PLUS'-1''2'))\0"};
  tokenise(p,str_34,0);
  assert(Prog(p));
  assert(strsame(p->flags.output_int, "1"));

  free(p);
}
