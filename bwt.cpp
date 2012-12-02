#include <vector>
#include <algorithm>
#include <assert.h>
#include "definitions.h"
using namespace std;

void suffixArrayBuild(const vector<byte> &, vector<int> &);

int BWTEncode(const vector<byte> & in, vector<byte> & out)
{
	assert(in.size() == out.size());

	int size = (int)in.size();
	vector<int> suffix_array(size);
	suffixArrayBuild(in, suffix_array);
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
	return lastBytePosition;
}

void suffixArrayBuild(const vector<byte> & in, vector<int> & p)
{
	assert(in.size() == p.size());

	int size = (int)in.size(); 
	vector<int> cnt(max(size, alphabet));
	vector<int> c(size);
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

	vector<int> pn(size);
	vector<int> cn(size);
	for (int h=0; (int)((size_t)1<<h)<size; ++h)
	{
		for (int i=0; i<size; ++i)
		{
			pn[i] = p[i] - ((size_t)1<<h);
			if (pn[i] < 0)  pn[i] += size;
		}
		cnt.assign(classes, 0);
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
		c.assign(cn.begin(), cn.end());
	}
}

void BWTDecode(const vector<byte> & in, vector<byte> & out, int lastBytePosition)
{
	assert(in.size() == out.size());

	int size = (int)in.size();
	vector<int> c(alphabet);
	vector<int> d(alphabet);
	vector<int> p(size);
	for (int i=0; i<size; ++i)
		p[i] = c[in[i]]++;
	for (int i=1; i<alphabet; ++i)
		d[i] = d[i-1] + c[i-1];
	vector<int> t(size);
	for (int i=0; i<size; ++i)
		t[i] = d[in[i]] + p[i];
	for (int i=size-1; i>=0; --i)
	{
		out[i] = in[lastBytePosition];
		lastBytePosition = t[lastBytePosition];
	}
}