#pragma once
#ifndef mtf
#define mtf

#include "definitions.h"
#include <vector>
using namespace std;

void MTFEncode(vector<byte> & block);
void MTFDecode(vector<byte> & block);

#endif