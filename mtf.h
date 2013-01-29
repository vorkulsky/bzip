#pragma once
#ifndef mtf
#define mtf

#include "definitions.h"

void MTFDecode(byte* block, int size);
void MTFEncode(byte* block, int size);

#endif