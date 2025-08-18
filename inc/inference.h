#ifndef __INFERENCE_H
#define __INFERENCE_H

#include "utils.h"
#include <stdlib.h>
#include <stdio.h>

char* get_hint(struct vector*, int**, int**, int);

char* next_tok(struct vector*, const struct table);

#endif
