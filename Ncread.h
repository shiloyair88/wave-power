#pragma once
#include "interpolation.h"
size_t read(const char *filename, const char *varname, point **dataout);
size_t *read2(const char *filename, const char *varname, point **dataout)