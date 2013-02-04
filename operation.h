#pragma once
#ifndef operation
#define operation

#include <stdio.h>
#include "bzipthread.h"
using namespace std;

class Operation
{
protected:
	const char* _inFileName;
	const char* _outFileName;
	int _threadsQuantity;
	FILE* _inFile;
	FILE* _outFile;
	// _inFileSize - ���������� ��� �� ����������� ������ � ����� _in.
	// �� ������ ������ �������� ������ �����.
	long _inFileSize;
	thread* _threads;
	threadVariables* _vars;
	bzipArguments* _args;
	// _threadNum �������� ����� �������� ��������������� ������� ������� ������.
	int _threadNum;
	// _end ���������� �� ���������� �� ����.
	bool _end;
public:
	Operation(const char* inFileName, const char* outFileName, int threadsQuantity);
	virtual ~Operation();
	/**
	*	��������� �����. ���������� �������� �� ������ � �������.
	*/
	void performOperation();
private:
	/**
	*	��������� ����� � ������� _inFileName � _outFileName �������������� ��� ������
	*	� ��� ������ � in � out. ���������� ������ ����� _inFileName.
	*	���� ���������� ������, ������� ��������� � ��������� ���������.
	*/
	void openFiles();
protected:
	/**
	*	� ��������� ������ ���������� ����� �������� ��������� ���-���� ����� �������� ������, ��
	*	�� ���������� ��������� ����� ���������. ������������� ���� �����, ��� ������ ������� ���.
	*/
	virtual void afterOpeningFiles() = 0;
	/**
	*	��������� ����� ����� � ��������� � ���� ��������� ������.
	*/
	virtual void threadRun() = 0;
	/**
	*	��������� �� �������� ����� ������ ���������� ����� � _args �������� ������. � �������� ������ ��� ���.
	*	������������� _end, ���� ��������� ����� �����.
	*/
	virtual void readData() = 0;
	/**
	*	���������� � �������� ���� ������� ������ �� _args �������� ������. � ����������� ������ �� ��� ���.
	*/
	virtual void writeData() = 0;
};

#endif