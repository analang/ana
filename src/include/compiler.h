#ifndef COMO_COMPILER_H
#define COMO_COMPILER_H

#include "vm.h"

ana_function *ana_compileast(char *filename, ana_vm *vm, node *ast);

#endif