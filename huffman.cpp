#include <string.h>
#include <vector>
#include <queue>
#include <algorithm>
#include "definitions.h"
using namespace std;

struct HuffmanTreeNode
{
	int value;
	int weight;
	HuffmanTreeNode *pleft, *pright;
	HuffmanTreeNode(): pleft(0), pright(0), value(0), weight(0) {}
};

void bytesWeightsCalculate(const byte* block, int size, int weight[]);
HuffmanTreeNode* HuffmanTreeBuild(const int weight[]);
void codesLengthsCalculate(HuffmanTreeNode* tree, byte* codesLengths);
void walkTree(HuffmanTreeNode* p, byte* codesLengths, byte level);
void removeHuffmanTree(HuffmanTreeNode* p);
void getCanonnicalCodes(const byte* codesLengths, byte canonnicalCodes[]);
void getSymb(byte symb[], const byte* codesLengths);
byte getNumberOfCharactersWithCodeLength(int numberOfCharactersWithCodeLength[], const byte* codesLengths);
byte* getHuffmanEncodedBlock(const byte* in, int inSize, int& outSize,
							 const byte* codesLengths, const byte canonnicalCodes[]);
byte getBitWithNumber(const byte* in, int inSize, int index);

struct greaterNode
{
	bool operator()(HuffmanTreeNode* x, HuffmanTreeNode* y)  
	{   
		return x->weight > y->weight;
	}
};

/**
*	Предикат сравнения символов алфавита по длине кодов Хаффмана, им соответствующих.
*/
struct Pred
{
	bool operator() (byte x, byte y)  
	{   
		return _codesLengths[x] > _codesLengths[y];
	}
	Pred(const byte* codesLengths): _codesLengths(codesLengths) {};
	const byte* _codesLengths;
};

byte* HuffmanEncode(const byte* in, int inSize, int& outSize, byte* codesLengths)
{
	//	weight[i] - количество символов i в in. 
	int weight[ALPHABET];
	memset(weight, 0, ALPHABET * sizeof(int));
	bytesWeightsCalculate(in, inSize, weight);
	HuffmanTreeNode* tree = HuffmanTreeBuild(weight);
	codesLengthsCalculate(tree, codesLengths);
	removeHuffmanTree(tree);
	//	canonnicalCodes[i] - канонический код Хаффмана символа i.
	byte canonnicalCodes[ALPHABET];
	getCanonnicalCodes(codesLengths, canonnicalCodes);
	byte* out = getHuffmanEncodedBlock(in, inSize, outSize, codesLengths, canonnicalCodes);
	return out;
}

/**
*	Определяет вес каждого символа, подсчитывая количество его вхождений в тексте.
*/
void bytesWeightsCalculate(const byte* block, int size, int weight[])
{
	for (int i=0; i<size; ++i)
		++weight[block[i]];
}

/**
*	Строи дерево Хаффмана, используя информацию о весах символов.
*	Возвращает указатель на корень.
*/
HuffmanTreeNode* HuffmanTreeBuild(const int weight[])
{
	priority_queue<HuffmanTreeNode*, vector<HuffmanTreeNode*>, greaterNode> pq;
	for (int i=0; i<ALPHABET; ++i)
	{
		if (weight[i])
		{
			HuffmanTreeNode *node = new HuffmanTreeNode;
			node->value = (byte)i;
			node->weight = weight[i];
			pq.push(node);
		}
	}
	if (pq.empty()) return 0;
	if (pq.size() == 1)
	{
		//	Обработка исключительного случая - строится дерево Хаффмана для строки, состоящей
		//	из одного или нескольких одинаковых символов, других символов нет.
		HuffmanTreeNode *n1 = pq.top();
		pq.pop();
		HuffmanTreeNode *n0 = new HuffmanTreeNode;
		n0->value = -1;
		n0->weight = n1->weight;
		n0->pleft = n1;
		return n0;
	}
	while (pq.size() > 1)
	{
		HuffmanTreeNode *n1 = pq.top();
		pq.pop();
		HuffmanTreeNode *n2 = pq.top();
		pq.pop();
		HuffmanTreeNode *node = new HuffmanTreeNode;
		node->weight = n1->weight + n2->weight;
		node->value = -1;
		node->pleft = n1;
		node->pright = n2;
		pq.push(node);
	}
	HuffmanTreeNode *n0 = pq.top();
	pq.pop();
	return n0;
}

/**
*	Подсчитывает длину кода каждого символа, садаваемого деревом Хаффмана.
*/
void codesLengthsCalculate(HuffmanTreeNode* tree, byte* codesLengths)
{
	memset(codesLengths, 0, ALPHABET * sizeof(byte));
	walkTree(tree, codesLengths, 0);
}

/**
*	Рекурсивно обходит дерево, подсчитывая расстояние от корня до каждого символа -
*	это и есть длина кода Хаффмана символа.
*/
void walkTree(HuffmanTreeNode* p, byte* codesLengths, byte level)
{
	if(p)
	{
		walkTree(p->pleft, codesLengths, level+1);
		if (p->value != -1)
			codesLengths[p->value] = level;
		walkTree(p->pright, codesLengths, level+1);
	}
}

/**
*	Рекурсивно удаляет дерево Хаффмана.
*/
void removeHuffmanTree(HuffmanTreeNode* p)
{
	if(p)
	{
		removeHuffmanTree(p->pleft);
		removeHuffmanTree(p->pright);
		delete p;
	}
}

/**
*	Строит канонические коды символов, используя информацио о длинах кода.
*/
void getCanonnicalCodes(const byte* codesLengths, byte canonnicalCodes[])
{
	//	symb[] - перестановка символов алфавита, отсортированная по двум ключам:
	//	первичный - длина кода, вторичный - лексикографическое значение.
	byte symb[ALPHABET];
	getSymb(symb, codesLengths);

	//	numberOfCharactersWithCodeLength[i] - количество символов алфавита, имеющих длину кода, равную i.
	//	!!!Важно: T хранит числа до 256 включительно, поэтому int.
	int numberOfCharactersWithCodeLength[ALPHABET];
	byte greatestCodeLength = 
		getNumberOfCharactersWithCodeLength(numberOfCharactersWithCodeLength, codesLengths);
	
	//	S[i] - начальное значение кода длины i, для всех i из [1..greatestCodeLength]
	byte S[ALPHABET];
	S[greatestCodeLength] = 0;
	for (int i=greatestCodeLength-1; i>0; --i)
		S[i] = (S[i+1] + numberOfCharactersWithCodeLength[i+1]) >> 1;

	memset(canonnicalCodes, 0, ALPHABET * sizeof(byte));

	for (int i=0; i<ALPHABET && codesLengths[symb[i]]; ++i)
		canonnicalCodes[symb[i]] = S[codesLengths[symb[i]]]++;
}

/**
*	Строит перестановку symb символов алфавита, отсортированную по двум ключам:
*	первичный - длина кода, вторичный - лексикографическое значение.
*	symb[] должен содержать место под ALPHABET элементов.
*/
void getSymb(byte symb[], const byte* codesLengths)
{
	for (int i=0; i<ALPHABET; ++i)
		symb[i]=(byte)i;
	Pred p(codesLengths);
	stable_sort(symb, symb + ALPHABET, p);
}

/**
*	Заполняет массив numberOfCharactersWithCodeLength[i] - количество символов алфавита, имеющих длину кода, равную i.
*	Ворзвращает через return наибольшую длину кода.
*	numberOfCharactersWithCodeLength[] должен содержать место под ALPHABET элементов.
*/
byte getNumberOfCharactersWithCodeLength(int numberOfCharactersWithCodeLength[], const byte* codesLengths)
{
	byte greatestCodeLength = 0;
	memset(numberOfCharactersWithCodeLength, 0, ALPHABET * sizeof(int));
	for (int i=0; i<ALPHABET; ++i)
	{
		++numberOfCharactersWithCodeLength[codesLengths[i]];
		if (codesLengths[i] > greatestCodeLength)
			greatestCodeLength = codesLengths[i];
	}
	return greatestCodeLength;
}

/**
*	Кодирует блок данных in размера inSize каноническими кодами Хаффмана canonnicalCodes[]
*	длины codesLengths.
*	Результат - закодированный блок возвращается через return. Размер результата возвращается в outSize.
*/
byte* getHuffmanEncodedBlock(const byte* in, int inSize, int& outSize,
						   const byte* codesLengths, const byte canonnicalCodes[])
{
	//	Нельзя заранее знать размер закодированных блока данных. Поэтому используется вектор.
	//	А потом результат копируется в массив, по размеру равный итоговому размеру вектора.
	vector<byte> outVector;
	//	Последовательно вычисляются биты закодированных символов и упаковываются в байты.
	//	_byte - заполняемый байт.
	byte _byte = 0;
	//	pos - текущая позиция в байте.
	int pos = 7;
	for (int i=0; i<inSize; i++)
	{
		for (int j=codesLengths[in[i]]; j>0; --j)
		{
			byte bit = canonnicalCodes[in[i]]>>(j-1) & 1;
			_byte = _byte | bit<<pos;
			if (pos == 0)
			{
				outVector.push_back(_byte);
				pos = 7;
				_byte = 0;
			}
			else --pos;
		}
	}
	if (pos!=7)
		outVector.push_back(_byte);

	outSize = (int)outVector.size();
	byte* out = new byte[outSize];
	copy(outVector.begin(), outVector.end(), out);
	return out;
}

void HuffmanDecode(const byte* in, int inSize, byte* out, int blockSize, const byte* codesLengths)
{
	//	symb[] - перестановка символов алфавита, отсортированная по двум ключам:
	//	первичный - длина кода, вторичный - лексикографическое значение.
	byte symb[ALPHABET];
	getSymb(symb, codesLengths);

	//	numberOfCharactersWithCodeLength[i] - количество символов алфавита, имеющих длину кода, равную i.
	//	!!!Важно: T хранит числа до 256 включительно, поэтому int.
	int numberOfCharactersWithCodeLength[ALPHABET];
	byte greatestCodeLength = 
		getNumberOfCharactersWithCodeLength(numberOfCharactersWithCodeLength, codesLengths);

	//	base[i] - количество нелистовых узлов дерева Хаффмана на уровне i.
	byte base[ALPHABET+1]; // Достаточный размер greatestCodeLength + 1.
	memset(base, 0, (ALPHABET+1) * sizeof(byte));
	//	offs[i] - индекс массива symb[], такой что symb[offs[i]] первый листовой узел (символ) на уровне i.
	byte offs[ALPHABET+1]; // Достаточный размер greatestCodeLength + 1.
	base[greatestCodeLength] = 0;
	offs[greatestCodeLength] = 0;
	for (int i=greatestCodeLength-1; i>0; --i)
	{
		base[i] = (numberOfCharactersWithCodeLength[i+1] + base[i+1]) >> 1;
		offs[i] = numberOfCharactersWithCodeLength[i+1] + offs[i+1];
	}

	int bitNumber = 0;
	int numberOfDecodedByte = 0;
	while (numberOfDecodedByte < blockSize)
	{
		byte code = getBitWithNumber(in, inSize, bitNumber++);
		byte length = 1;
		while (code < base[length])
		{
			code <<= 1;
			code += getBitWithNumber(in, inSize, bitNumber++);
			++length;
		}
		out[numberOfDecodedByte] = symb[offs[length] + code - base[length]];
		++numberOfDecodedByte;
	}
}

/**
*	Возвращает в байте бит с номером index из вектора байтов.
*/
byte getBitWithNumber(const byte* in, int inSize, int index)
{
	int byteNumber = index / 8;
	if (byteNumber > inSize)
		throw logic_error("Unexpected end of block.");
	byte b = in[byteNumber];
	return b>>(7 - index % 8) & 1;
}