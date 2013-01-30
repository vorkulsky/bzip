#pragma once
#ifndef huffman
#define huffman

#include "definitions.h"
#include <vector>
using namespace std;

/**
*	В in получает ссылку на заполенный входной массив. inSize - его размер. out - выходной массив,
*	возвращается через return. out содержит закодированные алгоритмом Хаффмана данные.
*	Память для out выделяется внутри функции, но отвечает за ее освобождение вызывающий. outSize - размер out.
*	codesLengths - доп. сведения, необходимые для декодирвоания. Память на 256 элементов выделяет и освобождает
*	вызывающий.
*/
byte* HuffmanEncode(const byte* in, int inSize, int& outSize, byte* codesLengths);

/**
*	В in получает ссылку на заполенный входной массив. inSize - его размер. out - выходной массив, 
*	содержит раскодированные данные. outSize - размер out. Память выделяет вызывающий.
*	codesLengths - доп. сведения, необходимые для декодирвоания. Должен содержать соответствующие 256 элементов.
*/
void HuffmanDecode(const byte* in, int inSize, byte* out, int outSize, const byte* codesLengths);

#endif