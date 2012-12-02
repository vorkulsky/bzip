#include <algorithm>
#include "definitions.h"
#include "bwt.h"
using namespace std;

void suffixArrayBuild(byte* in, int * p, int size);

int BWTEncode(byte* in, byte* out, int size)
{
	int *suffix_array = new int[size];
	suffixArrayBuild(in, suffix_array, size);
	int lastBytePosition = 0;
	for (int i=0; i<size; ++i)
	{
		if (suffix_array[i])
			out[i] = in[suffix_array[i]-1];
		else
		{
			out[i] = in[size-1];
			lastBytePosition = i;
		}
	}
	delete [] suffix_array;
	return lastBytePosition;
}

void suffixArrayBuild(byte* in, int * p, int size)
{
	int* cnt = new int[max(size, alphabet)];
	int* c = new int[size];
	memset(cnt, 0, alphabet * sizeof(int));
	for (int i=0; i<size; ++i)
		++cnt[in[i]];
	for (int i=1; i<alphabet; ++i)
		cnt[i] += cnt[i-1];
	for (int i=0; i<size; ++i)
		p[--cnt[in[i]]] = i;
	c[p[0]] = 0;
	int classes = 1;
	for (int i=1; i<size; ++i)
	{
		if (in[p[i]] != in[p[i-1]]) ++classes;
		c[p[i]] = classes-1;
	}

	int* pn = new int[size];
	int* cn = new int[size];
	for (int h=0; (int)((size_t)1<<h)<size; ++h)
	{
		for (int i=0; i<size; ++i)
		{
			pn[i] = p[i] - ((size_t)1<<h);
			if (pn[i] < 0)  pn[i] += size;
		}
		memset(cnt, 0, classes * sizeof(int));
		for (int i=0; i<size; ++i)
			++cnt[c[pn[i]]];
		for (int i=1; i<classes; ++i)
			cnt[i] += cnt[i-1];
		for (int i=size-1; i>=0; --i)
			p[--cnt[c[pn[i]]]] = pn[i];
		cn[p[0]] = 0;
		classes = 1;
		for (int i=1; i<size; ++i)
		{
			int mid1 = (p[i] + ((size_t)1<<h)) % size,  mid2 = (p[i-1] + ((size_t)1<<h)) % size;
			if (c[p[i]] != c[p[i-1]] || c[mid1] != c[mid2])
				++classes;
			cn[p[i]] = classes-1;
		}
		memcpy(c, cn, size * sizeof(int));
	}
	delete [] cn;
	delete [] pn;
	delete [] c;
	delete [] cnt;
}

void BWTDecode(byte* in, byte* out, int size, int lastBytePosition)
{
	int c[alphabet], d[alphabet];
	memset(c, 0, alphabet * sizeof(int));
	memset(d, 0, alphabet * sizeof(int));
	int* p = new int[size];
	for (int i=0; i<size; ++i)
		p[i] = c[in[i]]++;
	for (int i=1; i<alphabet; ++i)
		d[i] = d[i-1] + c[i-1];
	int* t = new int[size];
	for (int i=0; i<size; ++i)
		t[i] = d[in[i]] + p[i];
	delete [] p;
	for (int i=size-1; i>=0; --i)
	{
		out[i] = in[lastBytePosition];
		lastBytePosition = t[lastBytePosition];
	}
	delete [] t;
}