#ifndef __TRAINING_H
#define __TRAINING_H

#include "translation.h"
#include <stdlib.h>
#include <stdio.h>

bool table_depfx(struct table*);
bool table_pfx(struct table*);

bool table_push(struct table*, const int, const int);
bool read_data(struct table*, FILE*);

#endif
