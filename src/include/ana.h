#ifndef COMO_OBJECT_BASE
#define COMO_OBJECT_BASE

/** 
 * This file is the core header for the Como Object API. 
 * Only include this file to get all symbol defns. If you try to include
 * any of them directly, an error will occurr
 */
#define COMO_BASE_INCLUDED 1

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#define ANA_RUNTIME_MSG_MAX 1024UL

/* Order is important here */
#include "anaexceptions.h"
#include "utils.h"
#include "system.h"
#include "anaobject.h"
#include "frame.h"
#include "function.h"
#include "vm.h"
#include "anabool.h"
#include "container.h"
#include "class.h"
#include "array.h"
#include "module.h"
#include "anastring.h"
#include "map.h"
#include "long.h"
#include "double.h"
#include "code.h"
#include "arena.h"

#define LIKELY(x)       __builtin_expect(!!(x), 1)
#define UNLIKELY(x)     __builtin_expect(!!(x), 0)


COMO_OBJECT_API char *ana_get_fn_name(ana_frame * frame);

extern ana_vm *ana_vm_location(void);

#define ana_VM (ana_vm_location())


#endif
