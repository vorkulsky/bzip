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
	HuffmanTreeNode(): pleft(0), pright(0) {}
};

void bytesWeightsCalculate(const byte* block, int size, int weight[]);
HuffmanTreeNode* HuffmanTreeBuild(const int weight[]);
void codesLengthsCalculate(HuffmanTreeNode* tree, byte* codesLengths);
void walkTree(HuffmanTreeNode* p, byte* codesLengths, byte level);
void removeHuffmanTree(HuffmanTreeNode* p);
void getCanonnicalCodes(const byte* codesLengths, byte canonnicalCodes[]);
void getHuffmanEncodedBlock(vector<byte> & encoded_block, const byte* codesLengths,
							const byte canonnicalCodes[], const byte* block, int size);
byte getBitWithNumber(const byte* in, int index);

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
	vector<byte> outVector;
	getHuffmanEncodedBlock(outVector, codesLengths, canonnicalCodes, in, inSize);
	outSize = (int)outVector.size();
	byte* out = new byte[outSize];
	copy(outVector.begin(), outVector.end(), out);
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
	if (!pq.size()) return 0;
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
	byte L = 0;
	//	!!!Важно: T хранит числа до 256 включительно, поэтому int.
	int numberOfCharactersWithCodeLength[ALPHABET];
	memset(numberOfCharactersWithCodeLength, 0, ALPHABET * sizeof(int));
	for (int i=0; i<ALPHABET; ++i)
	{
		++numberOfCharactersWithCodeLength[codesLengths[i]];
		if (codesLengths[i] > L) L = codesLengths[i];
	}
	byte S[ALPHABET+1]; // Достаточен размер L+1;
	S[L] = 0;
	for (int i=L-1; i>0; --i)
		S[i] = (S[i+1] + numberOfCharactersWithCodeLength[i+1]) >> 1;
	byte symb[ALPHABET];
	for (int i=0; i<ALPHABET; ++i)
		symb[i]=(byte)i;
	Pred p(codesLengths);
	stable_sort(symb, symb + ALPHABET, p);
	for (int i=0; i<ALPHABET && codesLengths[symb[i]]; ++i)
		canonnicalCodes[symb[i]] = S[codesLengths[symb[i]]]++;
}

void getHuffmanEncodedBlock(vector<byte> & encoded_block, const byte* codesLengths,
							const byte canonnicalCodes[], const byte* block, int size)
{
	int pos = 7;
	int tmp = 0;
	for (int i=0; i<size; i++)
		for (int j=codesLengths[block[i]]; j>0; --j)
		{
			int f = canonnicalCodes[block[i]]>>(j-1) & 1;
			tmp = tmp | f<<pos;
			if (pos == 0)
			{
				encoded_block.push_back(tmp);
				pos = 7;
				tmp = 0;
			}
			else --pos;
		}
		if (pos!=7)
			encoded_block.push_back(tmp);
}

void HuffmanDecode(const byte* in, int inSize, byte* out, int blockSize, const byte* codesLengths)
{
	//	symb[] - перестановка символов алфавита, отсортированная по двум ключам:
	//	первичный - длина кода, вторичный - лексикографическое значение.
	byte symb[ALPHABET];
	for (int i=0; i<ALPHABET; ++i)
		symb[i] = (byte)i;
	Pred p(codesLengths);
	stable_sort(symb, symb + ALPHABET, p);

	//	greatestCodeLength - наибольшая длина кода.
	byte greatestCodeLength = 0;
	//	numberOfCharactersWithCodeLength[i] - количество символов алфавита, имеющих длину кода, равную i.
	//	!!!Важно: хранит числа до 256 включительно, поэтому int.
	int numberOfCharactersWithCodeLength[ALPHABET];
	memset(numberOfCharactersWithCodeLength, 0, ALPHABET * sizeof(int));
	for (int i=0; i<ALPHABET; ++i)
	{
		++numberOfCharactersWithCodeLength[codesLengths[i]];
		if (codesLengths[i] > greatestCodeLength)
			greatestCodeLength = codesLengths[i];
	}

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
		byte code = getBitWithNumber(in, bitNumber++);
		byte length = 1;
		while (code < base[length])
		{
			code <<= 1;
			code += getBitWithNumber(in, bitNumber++);
			++length;
		}
		out[numberOfDecodedByte] = symb[offs[length] + code - base[length]];
		++numberOfDecodedByte;
	}
}

/**
*	Возвращает в байте бит с номером index из вектора байтов.
*/
byte getBitWithNumber(const byte* in, int index)
{
	int byteNumber = index / 8;
	byte b = in[byteNumber];
	return b>>(7 - index % 8) & 1;
}