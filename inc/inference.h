#ifndef __INFERENCE_H
#define __INFERENCE_H

#include "utarray.h"
#include "training.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

/* Given a history vector and a table, predict the next token */
char* next_tok(UT_array*, const UT_array*, const UT_array*);

#endif
