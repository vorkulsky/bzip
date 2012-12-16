#include <vector>
#include <queue>
#include <algorithm>
#include <assert.h>
#include <iostream>
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

void getCanonnicalCodes(const vector<byte> & codesLengths, vector<byte> & canonnicalCodes)
{
	byte L = 0;
	vector<byte> T(alphabet);
	for (int i=0; i<alphabet; ++i)
	{
		++T[codesLengths[i]];
		if (codesLengths[i] > L) L = codesLengths[i];
	}
	vector<byte> S(L+1);
	S[L] = 0;
	for (int i=L-1; i>0; --i)
		S[i] = (S[i+1] + T[i+1]) >> 1;
	vector<byte> symb(alphabet);
	for (int i=0; i<alphabet; ++i)
		symb[i]=(byte)i;
	Pred p(codesLengths);
	stable_sort(symb.begin(), symb.end(), p);
	for (int i=0; i<alphabet && codesLengths[symb[i]]; ++i)
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

void HuffmanEncode(const vector<byte> & in, vector<byte> & out, vector<byte> & codesLengths)
{
	assert(codesLengths.size() == alphabet);
	assert(out.size() == 0);

	vector<int> weight(alphabet);
	bytesWeightsCalculate(in, weight);
	HuffmanTreeNode* tree = HuffmanTreeBuild(weight);
	codesLengthsCalculate(tree, codesLengths);
	removeHuffmanTree(tree);
	vector<byte> canonnicalCodes(256);
	getCanonnicalCodes(codesLengths, canonnicalCodes);
	getHuffmanEncodedBlock(out, codesLengths, canonnicalCodes, in);
}

byte getBitWithNumber(const vector<byte> & in, int index)
{
	int byteNumber = index / 8;
	byte b = in[byteNumber];
	return b>>(7 - index % 8) & 1;
}

void HuffmanDecode(const vector<byte> & in, int blockSize, vector<byte> & out, const vector<byte> & codesLengths)
{
	assert(codesLengths.size() == alphabet);
	assert((int)out.size() == blockSize);

	byte L = 0;
	vector<byte> T(alphabet);
	for (int i=0; i<alphabet; ++i)
	{
		++T[codesLengths[i]];
		if (codesLengths[i] > L) L = codesLengths[i];
	}
	vector<byte> base(L+1);
	vector<byte> offs(L+1);
	base[L] = 0;
	offs[L] = 0;
	for (int i=L-1; i>0; --i)
	{
		base[i] = (T[i+1]+base[i+1])>>1;
		offs[i] = T[i+1] + offs[i+1];
	}
	vector<byte> symb(alphabet);
	for (int i=0; i<alphabet; ++i)
		symb[i]=(byte)i;
	Pred p(codesLengths);
	stable_sort(symb.begin(), symb.end(), p);
	int i = 0;
	int j = 0;
	while (j < blockSize)
	{
		byte code = getBitWithNumber(in, i++);
		byte length = 1;
		while (code < base[length])
		{
			code <<= 1;
			code += getBitWithNumber(in, i++);
			++length;
		}
		out[j] = symb[offs[length] + code - base[length]];
		++j;
	}
}
