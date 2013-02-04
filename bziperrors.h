#pragma once
#ifndef bziperrors
#define bziperrors

// Все функции выводят сообщение на экран. Все, кроме readError. завершают работу программы.
void fileNotFoundError(const char* fileName);
void fileNotOpenedError(const char* fileName);
void readError(const char* fileName);
void identicalAddressesError(const char* fileName1, const char* fileName2);

#endif