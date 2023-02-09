#pragma once

#include "lisp.h"

#define LISTSTRLEN 1000
#define MAXLIST 1000
#define EMPTY '\0'
#define LISPIMPL "Linked"

struct lisp{
  lisp* car;
  atomtype i;
  lisp* cdr;
};
