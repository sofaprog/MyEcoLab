/*
 * <кодировка символов>
 *   Cyrillic (UTF-8 with signature) - Codepage 65001
 * </кодировка символов>
 *
 * <сводка>
 *   EcoLab1
 * </сводка>
 *
 * <описание>
 *   Данный исходный файл является точкой входа
 * </описание>
 *
 * <автор>
 *   Copyright (c) 2018 Vladimir Bashev. All rights reserved.
 * </автор>
 *
 */

/* Eco OS */
#include "IEcoSystem1.h"
#include "IdEcoMemoryManager1.h"
#include "IdEcoInterfaceBus1.h"
#include "IdEcoFileSystemManagement1.h"
#include "IdEcoLab1.h"
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>   
#include <string.h> 

int __cdecl compare_int(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

/* Функция для проверки, что массив отсортирован */
int is_array_sorted(int32_t* arr, int size) {
    int i;
    for (i = 0; i < size - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            return 0;
        }
    }
    return 1;
}

/* Тестирование на целых числах */
void test_integer_sort(IEcoLab1* pIEcoLab1, IEcoMemoryAllocator1* pIMem, int size, const char* case_type) {
    int32_t* my_array = (int32_t*)pIMem->pVTbl->Alloc(pIMem, size * sizeof(int32_t));
    int32_t* qsort_array = (int32_t*)pIMem->pVTbl->Alloc(pIMem, size * sizeof(int32_t));
    clock_t start, end;
    double my_time, qsort_time;
    int i;
    int my_sorted, qsort_sorted;
    
    
    /* Генерация данных в зависимости от случая */
    if (strcmp(case_type, "random") == 0) {
        for (i = 0; i < size; i++) {
            my_array[i] = rand() % 1000000;
            qsort_array[i] = my_array[i];
        }
    } else if (strcmp(case_type, "sorted") == 0) {
        for (i = 0; i < size; i++) {
            my_array[i] = i;
            qsort_array[i] = i;
        }
    } else if (strcmp(case_type, "reverse") == 0) {
        for (i = 0; i < size; i++) {
            my_array[i] = size - i;
            qsort_array[i] = size - i;
        }
    }
    
    /* Тестируем свою сортировку */
    start = clock();
    pIEcoLab1->pVTbl->Sort(pIEcoLab1, my_array, size);
    end = clock();
    my_time = ((double)(end - start)) * 1000 / CLOCKS_PER_SEC;
    
    my_sorted = is_array_sorted(my_array, size);
    
    start = clock();
    qsort(qsort_array, size, sizeof(int32_t), compare_int);
    end = clock();
    qsort_time = ((double)(end - start)) * 1000 / CLOCKS_PER_SEC;
    
    qsort_sorted = is_array_sorted(qsort_array, size);
    
    /* Выводим результаты */
    printf("%d,%s,%s,%.2f,%.2f", size, case_type, 
           (my_sorted && qsort_sorted) ? "OK" : "ERROR", 
           my_time, qsort_time);
    
    if (!my_sorted) {
        printf(" [My sort not work!]");
    }
    printf("\n");
    
    pIMem->pVTbl->Free(pIMem, my_array);
    pIMem->pVTbl->Free(pIMem, qsort_array);
}

/*
 *
 * <сводка>
 *   Функция EcoMain
 * </сводка>
 *
 * <описание>
 *   Функция EcoMain - точка входа
 * </описание>
 *
 */
int16_t EcoMain(IEcoUnknown* pIUnk) {
    int16_t result = -1;
    /* Указатель на системный интерфейс */
    IEcoSystem1* pISys = 0;
    /* Указатель на интерфейс работы с системной интерфейсной шиной */
    IEcoInterfaceBus1* pIBus = 0;
    /* Указатель на интерфейс работы с памятью */
    IEcoMemoryAllocator1* pIMem = 0;
    char_t* name = 0;
    char_t* copyName = 0;
    /* Указатель на тестируемый интерфейс */
    IEcoLab1* pIEcoLab1 = 0;
    
    int sizes[] = {10000, 100000, 250000, 500000, 750000, 1000000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    const char* cases[] = {"random", "sorted", "reverse"};
    int num_cases = sizeof(cases) / sizeof(cases[0]);
    int s, c;  /* Счетчики для циклов */
    
    printf("Start testing EcoLab1\n");
    
    /* Проверка и создание системного интерфейса */
    if (pISys == 0) {
        result = pIUnk->pVTbl->QueryInterface(pIUnk, &GID_IEcoSystem, (void **)&pISys);
        if (result != 0 && pISys == 0) {
            goto Release;
        }
    }

    /* Получение интерфейса для работы с интерфейсной шиной */
    result = pISys->pVTbl->QueryInterface(pISys, &IID_IEcoInterfaceBus1, (void **)&pIBus);
    if (result != 0 || pIBus == 0) {
        goto Release;
    }
    
#ifdef ECO_LIB
    /* Регистрация статического компонента для работы со списком */
    result = pIBus->pVTbl->RegisterComponent(pIBus, &CID_EcoLab1, (IEcoUnknown*)GetIEcoComponentFactoryPtr_1F5DF16EE1BF43B999A434ED38FE8F3A);
    if (result != 0 ) {
        goto Release;
    }
#endif

    /* Получение интерфейса управления памятью */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoMemoryManager1, 0, &IID_IEcoMemoryAllocator1, (void**) &pIMem);

    /* Проверка */
    if (result != 0 || pIMem == 0) {
        goto Release;
    }

    /* Получение тестируемого интерфейса */
    result = pIBus->pVTbl->QueryComponent(pIBus, &CID_EcoLab1, 0, &IID_IEcoLab1, (void**) &pIEcoLab1);
    if (result != 0 || pIEcoLab1 == 0) {
        goto Release;
    }
    
    printf("Size,type,status,MySort(ms),Qsort(ms)\n");

    srand((unsigned int)time(NULL));

    for (s = 0; s < num_sizes; s++) {
        for (c = 0; c < num_cases; c++) {
            test_integer_sort(pIEcoLab1, pIMem, sizes[s], cases[c]);
        }
    }
    printf("Press Enter...\n");
    getchar();

Release:
    /* Освобождение интерфейса для работы с интерфейсной шиной */
    if (pIBus != 0) {
        pIBus->pVTbl->Release(pIBus);
    }

    /* Освобождение интерфейса работы с памятью */
    if (pIMem != 0) {
        pIMem->pVTbl->Release(pIMem);
    }

    /* Освобождение тестируемого интерфейса */
    if (pIEcoLab1 != 0) {
        pIEcoLab1->pVTbl->Release(pIEcoLab1);
    }

    /* Освобождение системного интерфейса */
    if (pISys != 0) {
        pISys->pVTbl->Release(pISys);
    }

    return result;
}