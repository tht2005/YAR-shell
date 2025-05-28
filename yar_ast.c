#include "yar_ast.h"

redirection make_redirection (int type, string file)
{
  redirection ret;
  ret.type = type;
  ret.file = file;
  return ret;
}
