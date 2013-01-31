#pragma once
#ifndef bzip
#define bzip

#include "definitions.h"

/**
*	in - заполненный входной массив данных. inSize - его размер.
*	Посредством return возвращается массив с закодированными данными out. outSize - его размер.
*	Память под out выделяется внутри функции, но за ее освобождение отвечает вызывающий.
*	lastBytePosition и codesLengths - доп. данные необходимые для декодирования.
*	Паямть под 256 элементов в codesLengths должда быть выделена и освобождена вызывающим.
*/
byte* BZIPEncode(const byte* in, int inSize, int& outSize, int& lastBytePosition, byte* codesLengths);

/**
*	in - заполненный входной массив данных. inSize - его размер.
*	out - выходной массив с декодированными данными. outSize - его размер.
*	Память под out выделяется и освобождается вызывающим.
*	lastBytePosition и codesLengths - доп. данные необходимые для декодирования.
*	сodesLengths должен содержать 256 соответствующих элементов. Память выделяется и освобождается вызывающим.
*/
void BZIPDecode(const byte* in, int inSize, byte* out, int outSize, int lastBytePosition, const byte* codesLengths);

#endif