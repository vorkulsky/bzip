#pragma once
#ifndef bwt
#define bwt

#include "definitions.h"

/**
*	in - заполненный входной массив данных.
*	out - массив, после выполения будет содержать закодированные данные.
*	Размер in и out равен size.
*	Память под out выделят и освобождает вызывающий.
*	Посредством return возвращает доп. данные, необходимые для декодирования.
*/
int BWTEncode(const byte* in, byte* out, int size);

/**
*	in - заполненный входной массив данных.
*	out - массив, после выполения будет содержать закодированные данные.
*	Размер in и out равен size.
*	Память под out выделят и освобождает вызывающий.
*	lastBytePosition - соответствующие доп. данные для декодирования.
*/
void BWTDecode(const byte* in, byte* out, int size, int lastBytePosition);

#endif