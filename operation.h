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
	// _inFileSize - количество еще не прочитанных байтов в файле _in.
	// До начала чтения содержит размер файла.
	long _inFileSize;
	thread* _threads;
	threadVariables* _vars;
	bzipArguments* _args;
	// _threadNum содержит номер текущего обрабатываемого глвыным потоком потока.
	int _threadNum;
	// _end показывает не закончился ли файл.
	bool _end;
public:
	Operation(const char* inFileName, const char* outFileName, int threadsQuantity);
	virtual ~Operation();
	/**
	*	Шаблонный метод. Производит операцию по работе с архивом.
	*/
	void performOperation();
private:
	/**
	*	Открывают файлы с именами _inFileName и _outFileName соответственно для чтения
	*	и для записи в in и out. Возвращает размер файла _inFileName.
	*	Если происходит ошибка, выводит сообщение и завершает программу.
	*/
	void openFiles();
protected:
	/**
	*	В шаблонном методе наследники могут захотеть выполнить что-либо после открытия файлов, но
	*	до выполнения следующих шагов алгоритма. Переопределив этот метод, они смогут сделать это.
	*/
	virtual void afterOpeningFiles() = 0;
	/**
	*	Запускает новый поток и загружает в него первичные данные.
	*/
	virtual void threadRun() = 0;
	/**
	*	Считывает из входного файла данные очередного блока в _args текущего потока. И выделяет память под них.
	*	Устанавливает _end, если достигнут конец файла.
	*/
	virtual void readData() = 0;
	/**
	*	Записывает в выходной файл готовые данные из _args текущего потока. И освобождает память из под них.
	*/
	virtual void writeData() = 0;
};

#endif