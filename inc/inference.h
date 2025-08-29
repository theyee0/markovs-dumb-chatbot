#ifndef __INFERENCE_H
#define __INFERENCE_H

#include "uthash.h"
#include "utarray.h"
#include "training.h" /* Move table functions to a new header later */
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

char* get_hint(UT_array*, int**, int**, int);

char* next_tok(UT_array*, const UT_array*, const UT_array*);

#endif
