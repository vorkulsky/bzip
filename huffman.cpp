#include <vector>
#include <queue>
#include <algorithm>
#include <assert.h>
#include "definitions.h"
using namespace std;

struct HuffmanTreeNode
{
	int value;
	int weight;
	HuffmanTreeNode *pleft, *pright;
	HuffmanTreeNode(): pleft(0), pright(0) {}
};

struct greaterNode
{
	bool operator()(HuffmanTreeNode* x, HuffmanTreeNode* y)  
	{   
		return x->weight > y->weight;
	}
};

/**
*	Предикат перестановки символов алфавита, отсортированная по двум ключам:
*	первичный - длина кода, вторичный - лексикографическое значение
*/
struct Pred
{
	bool operator() (byte x, byte y)  
	{   
		return codesLengths[x] > codesLengths[y];
	}
	Pred(const vector<byte> & _codesLengths): codesLengths(_codesLengths){};
	Pred& operator=(const Pred & other)
	{
		if (this != &other)
		{
			for (size_t i=0; i<other.codesLengths.size(); ++i)
				codesLengths.push_back(other.codesLengths[i]);
			return *this;
		}
	}
	vector<byte> codesLengths;
};

void bytesWeightsCalculate(const vector<byte> & block, vector<int> & weight);
HuffmanTreeNode* HuffmanTreeBuild(const vector<int> & weight);
void codesLengthsCalculate(HuffmanTreeNode* tree, vector<byte> & codesLengths);
void removeHuffmanTree(HuffmanTreeNode* p);
void walkTree(HuffmanTreeNode* p, vector<byte> & codesLengths, byte level);
void getCanonnicalCodes(const vector<byte> & codesLengths, vector<byte> & canonnicalCodes);
void getHuffmanEncodedBlock(vector<byte> & encoded_block, const vector<byte> & codesLengths,
							const vector<byte> & canonnicalCodes, const vector<byte> & block);
byte getBitWithNumber(const vector<byte> & in, int index);

void HuffmanEncode(const vector<byte> & in, vector<byte> & out, vector<byte> & codesLengths)
{
	assert(codesLengths.size() == ALPHABET);
	assert(out.size() == 0);

	vector<int> weight(ALPHABET);
	bytesWeightsCalculate(in, weight);
	HuffmanTreeNode* tree = HuffmanTreeBuild(weight);
	codesLengthsCalculate(tree, codesLengths);
	removeHuffmanTree(tree);
	vector<byte> canonnicalCodes(256);
	getCanonnicalCodes(codesLengths, canonnicalCodes);
	getHuffmanEncodedBlock(out, codesLengths, canonnicalCodes, in);
}

HuffmanTreeNode* HuffmanTreeBuild(const vector<int> & weight)
{
	priority_queue<HuffmanTreeNode*, vector<HuffmanTreeNode*>, greaterNode> pq;
	for (size_t i=0; i<weight.size(); ++i)
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

void walkTree(HuffmanTreeNode* p, vector<byte> & codesLengths, byte level)
{
	if(p)
	{
		walkTree(p->pleft, codesLengths, level+1);
		if (p->value != -1)
			codesLengths[p->value] = level;
		walkTree(p->pright, codesLengths, level+1);
	}
}

void codesLengthsCalculate(HuffmanTreeNode* tree, vector<byte> & codesLengths)
{
	walkTree(tree, codesLengths, 0);
}

/**
*	Определяет вес каждого символа, подсчитывая количество его вхождений в тексте.
*/
void bytesWeightsCalculate(const vector<byte> & block, vector<int> & weight)
{
	size_t size = block.size();
	for (size_t i=0; i<size; ++i)
		++weight[block[i]];
}

void removeHuffmanTree(HuffmanTreeNode* p)
{
	if(p)
	{
		removeHuffmanTree(p->pleft);
		removeHuffmanTree(p->pright);
		delete p;
	}
}

void getCanonnicalCodes(const vector<byte> & codesLengths, vector<byte> & canonnicalCodes)
{
	byte L = 0;
	vector<byte> T(ALPHABET);
	for (int i=0; i<ALPHABET; ++i)
	{
		++T[codesLengths[i]];
		if (codesLengths[i] > L) L = codesLengths[i];
	}
	vector<byte> S(L+1);
	S[L] = 0;
	for (int i=L-1; i>0; --i)
		S[i] = (S[i+1] + T[i+1]) >> 1;
	vector<byte> symb(ALPHABET);
	for (int i=0; i<ALPHABET; ++i)
		symb[i]=(byte)i;
	Pred p(codesLengths);
	stable_sort(symb.begin(), symb.end(), p);
	for (int i=0; i<ALPHABET && codesLengths[symb[i]]; ++i)
		canonnicalCodes[symb[i]] = S[codesLengths[symb[i]]]++;
}

void getHuffmanEncodedBlock(vector<byte> & encoded_block, const vector<byte> & codesLengths,
							const vector<byte> & canonnicalCodes, const vector<byte> & block)
{
	size_t size = block.size();
	int pos = 7;
	int tmp = 0;
	for (size_t i=0; i<size; i++)
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

void HuffmanDecode(const vector<byte> & in, vector<byte> & out, const vector<byte> & codesLengths)
{
	assert(codesLengths.size() == ALPHABET);

	int blockSize = (int)out.size();

	// symb[] - перестановка символов алфавита, отсортированная по двум ключам:
	// первичный - длина кода, вторичный - лексикографическое значение
	vector<byte> symb(ALPHABET);
	for (int i=0; i<ALPHABET; ++i)
		symb[i] = (byte)i;
	Pred p(codesLengths);
	stable_sort(symb.begin(), symb.end(), p);

	// greatestCodeLength - наибольшая длина кода.
	byte greatestCodeLength = 0;
	// numberOfCharactersWithCodeLength[i] - количество символов алфавита, имеющих длину кода, равную i.
	vector<byte> numberOfCharactersWithCodeLength(ALPHABET);
	for (int i=0; i<ALPHABET; ++i)
	{
		++numberOfCharactersWithCodeLength[codesLengths[i]];
		if (codesLengths[i] > greatestCodeLength)
			greatestCodeLength = codesLengths[i];
	}

	// base[i] - количество нелистовых узлов дерева Хаффмана на уровне i.
	vector<byte> base(greatestCodeLength + 1);
	// offs[i] - индекс массива symb[], такой что symb[offs[i]] первый листовой узел (символ) на уровне i.
	vector<byte> offs(greatestCodeLength + 1);
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
byte getBitWithNumber(const vector<byte> & in, int index)
{
	int byteNumber = index / 8;
	byte b = in[byteNumber];
	return b>>(7 - index % 8) & 1;
}