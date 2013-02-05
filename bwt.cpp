#include <string.h>
#include <algorithm>
#include "definitions.h"
using namespace std;

void suffixArrayBuild(const byte* in, int* permutation, int size);
int suffixArrayBuild_firstStep(const byte* in, int size, int* permutation, int* classesNumbers);
void countingSortPermutation(const byte* in, int size, int* permutation);
void countingSortPermutationOnSecondElementsOfPairs(const int* pn, int size, int* permutation,
                                                    const int* classesNumbers, int classes);

int BWTEncode(const byte* in, byte* out, int size)
{
    int* suffix_array = new int[size];
    suffixArrayBuild(in, suffix_array, size);
    int lastBytePosition = 0;
    for (int i=0; i<size; ++i)
    {
        if (suffix_array[i])
            out[i] = in[suffix_array[i] - 1];
        else
        {
            out[i] = in[size - 1];
            lastBytePosition = i;
        }
    }
    delete [] suffix_array;
    return lastBytePosition;
}

void BWTDecode(const byte* in, byte* out, int size, int lastBytePosition)
{
    //    c[] для каждой буквы алфавита содержит количество раз,
    //    которое она встречается в тексте.
    //    Требуется инициализация нулями.
    int c[ALPHABET];
    memset(c, 0, ALPHABET * sizeof(int));
    //    d[] для каждой буквы алфавита содержит количество раз,
    //    которое встречаются все меньшие ее буквы.
    //    Требуется инициализация 0-го элемента нулем.
    int d[ALPHABET];
    memset(d, 0, ALPHABET * sizeof(int));
    //    p[] для каждого символа текста in содержит количество раз,
    //    которое этот символ встречается раньше по тексту.
    int* p = new int[size];

    //    Заполняем с[], d[], и p[].
    for (int i=0; i<size; ++i)
        p[i] = c[in[i]]++;
    for (int i=1; i<ALPHABET; ++i)
        d[i] = d[i-1] + c[i-1];

    //    t[] - вектор перестановки.
    int* t = new int[size];
    for (int i=0; i<size; ++i)
        t[i] = d[in[i]] + p[i];

    delete [] p;

    //    Используя вектор перестановки, вычисляем все буквы текста от начала к концу.
    for (int i=size-1; i>=0; --i)
    {
        out[i] = in[lastBytePosition];
        lastBytePosition = t[lastBytePosition];
    }
    delete [] t;
}

/**
*    Строит суффиксный массив permutation текста in алгоритмом O(NlogN)
*    (применяя видоизмененную поразрядную сортировку к циклическим сдвигам)
*/
void suffixArrayBuild(const byte* in, int* permutation, int size)
{
    //    classesNumbers[i] для каждой циклической подстроки, начинающейся в позиции i с длиной 2^h,
    //    содержит номер класса эквивалентности, которому эта подстрока принадлежит.
    int* classesNumbers = new int[size];

    int classes = suffixArrayBuild_firstStep(in, size, permutation, classesNumbers);

    //    pn[] содержит перестановку по вторым элементам пар.
    int* pn = new int[size];
    //    newClassesNumbers[] содержит номера новых классов эквивалентности.
    int* newClassesNumbers = new int[size];

    for (int h=0; (1<<h)<size; ++h)
    {
        //    Сортировка по вторым элементам пар.
        for (int i=0; i<size; ++i)
        {
            pn[i] = permutation[i] - (1<<h);
            if (pn[i] < 0)
                pn[i] += size;
        }

        //    Стабильная сортировка подсчетом по первым элементам пар.
        countingSortPermutationOnSecondElementsOfPairs(pn, size, permutation, classesNumbers, classes);

        //    Проходом по вектору перестановки permutation, сравненивая соседние элементы как пары двух чисел,
        //    строим вектор номеров новых классов эквивалентности newClassesNumbers.
        newClassesNumbers[permutation[0]] = 0;
        classes = 1;
        for (int i=1; i<size; ++i)
        {
            int mid1 = (permutation[i] + (1<<h)) % size;
            int mid2 = (permutation[i-1] + (1<<h)) % size;
            if (classesNumbers[permutation[i]] != classesNumbers[permutation[i-1]] ||
                classesNumbers[mid1] != classesNumbers[mid2])
                    ++classes;
            newClassesNumbers[permutation[i]] = classes-1;
        }
        memcpy(classesNumbers, newClassesNumbers, size * sizeof(int));
    }

    delete [] newClassesNumbers;
    delete [] pn;
    delete [] classesNumbers;
}

/**
*    Выполняет первый шаг (инициализацию) алгоритма построения суффиксного массива.
*    Подсчетом сортирует циклические подстроки длины 1, т.е. отдельные символы,
*    и делит их на классы эквивалентности (одинаковые символы относит к одному классу эквивалентности).
*   На вход принимает строку in, для которой строится суффиксный массив, незаполненный вектор permutation и
*    незаполненный вектор номеров классов эквивалентности classesNumbers, имеющие то же количество элементов,
*    что и строка in. Заполняет вектора permutation и classesNumbers.
*    Возвращает количество классов эквивалентности.
*/
int suffixArrayBuild_firstStep(const byte* in, int size, int* permutation, int* classesNumbers)
{
    countingSortPermutation(in, size, permutation);

    //    Проходом по вектору перестановки permutation, сравненивая соседние символы,
    //    строим вектор номеров классов эквивалентности classesNumbers.
    classesNumbers[permutation[0]] = 0;
    int classes = 1;
    for (int i=1; i<size; ++i)
    {
        if (in[permutation[i]] != in[permutation[i-1]])
            ++classes;
        classesNumbers[permutation[i]] = classes - 1;
    }
    return classes;
}

/**
*    Подсчетом вычисляет такую перестановку permutation символов строки in,
*    после которой символы окажутся расставлены в алфавитном порядке. Сортировка может быть нестабильной.
*/
void countingSortPermutation(const byte* in, int size, int* permutation)
{
    //    сount[] массив подсчета одинаковых элементов при сортировке подсчетом.
    //    Требуется инициализация 0-го элемента нулем.
    int* count = new int[max(size, ALPHABET)];
    memset(count, 0, ALPHABET * sizeof(int));

    for (int i=0; i<size; ++i)
        ++count[in[i]];
    for (int i=1; i<ALPHABET; ++i)
        count[i] += count[i-1];
    for (int i=0; i<size; ++i)
        permutation[--count[in[i]]] = i;

    delete [] count;
}

/**
*    Подсчетом вычисляет такую перестановку по первым элементам пар permutation
*    перестановки pn по вторым элементам пар, после которой пары окажутся расставлены в алфавитном порядке.
*    Сортировка стабильна.
*/
void countingSortPermutationOnSecondElementsOfPairs(const int* pn, int size, int* permutation,
                                                    const int* classesNumbers, int classes)
{
    //    сount[] массив подсчета одинаковых (по классу эквивалентности classesNumbers)
    //    элементов при сортировке подсчетом. Требуется инициализация 0-го элемента нулем.
    int* count = new int[max(size, ALPHABET)];
    memset(count, 0, classes * sizeof(int));
    //    missKiller[] временный массив. Введен для порьбы с кеш промахами.
    int* missKiller = new int[size];
    
    /*
    *    for (int i=0; i<size; ++i)
    *        ++count[classesNumbers[pn[i]]];
    *    Этот код эквивалентен следующему.
    *    Он заменен для уменьшения количества кэш промахов.
    */
    for (int i=0; i<size; ++i)
        missKiller[i] = classesNumbers[pn[i]];
    for (int i=0; i<size; ++i)
        ++count[missKiller[i]];
    //    Конец эквивалентного кода.
    for (int i=1; i<classes; ++i)
        count[i] += count[i-1];
    /*
    *    for (int i=size-1; i>=0; --i)
    *        permutation[--count[classesNumbers[pn[i]]]] = pn[i];
    *    Этот код эквивалентен следующему.
    *    Он заменен для уменьшения количества кэш промахов.
    */
    for (int i=size-1; i>=0; --i)
        missKiller[i] = classesNumbers[pn[i]];
    for (int i=size-1; i>=0; --i)
        missKiller[i] = --count[missKiller[i]];
    for (int i=size-1; i>=0; --i)
        permutation[missKiller[i]] = pn[i];
    //    Конец эквивалентного кода.

    delete [] missKiller;
    delete [] count;
}
