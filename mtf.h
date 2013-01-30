#pragma once
#ifndef mtf
#define mtf

#include "definitions.h"

/**
*	block - заполненный массив входных данных. size - его размер.
*	Результат после выполения будет в block. Размер не изменится.
*/
void MTFDecode(byte* block, int size);

/**
*	block - заполненный массив входных данных. size - его размер.
*	Результат после выполения будет в block. Размер не изменится.
*/
void MTFEncode(byte* block, int size);

#endif