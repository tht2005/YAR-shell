#ifndef __YAR_AST_H
#define __YAR_AST_H

#include "data_structure/string.h"

typedef struct redirection {

  int type;
  string file;

} redirection;

redirection make_redirection (int type, string file);

#endif
