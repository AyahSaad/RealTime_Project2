#ifndef TEAM_H
#define TEAM_H

#include "team.h"
#include <stdio.h>
#include <pthread.h>
#include "fileReaders.h"

typedef struct
{
    int threadNum;
    int teamNum;
    int *condFlag;
    pthread_mutex_t *condMutex;
    pthread_cond_t *cond;
    int *nextProductIndex;
    pthread_barrier_t *barrier;
    int *productsCount;

} ThreadArgs;

typedef struct
{

    long mtype;
    int index;

} Notifier;

void *thread_function(void *arg);
void teamFunc(long type, int qid, int *totalInStock, pthread_mutex_t* totalInStockmutex);

#endif