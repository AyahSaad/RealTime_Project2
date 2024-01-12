#include <pthread.h>
#include "fileReaders.h"
#include "team.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

void teamFunc(long type, int qid, int *totalInStock, pthread_mutex_t *totalInStockmutex)
{
    pthread_t threads[NUM_EMPLOYEES_PER_TEAM];
    int thread_ids[NUM_EMPLOYEES_PER_TEAM];
    ThreadArgs args[NUM_EMPLOYEES_PER_TEAM];

    pthread_mutex_t condMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t task_available = PTHREAD_COND_INITIALIZER;
    int task_completed = 0;

    int productNext = 0;
    int productsCount = 0;

    pthread_barrier_t barrier;

    if (pthread_barrier_init(&barrier, NULL, NUM_EMPLOYEES_PER_TEAM + 1) != 0)
    {
        perror("pthread_barrier_init \n");
    }

    for (int i = 0; i < NUM_EMPLOYEES_PER_TEAM; i++)
    {
        thread_ids[i] = i;

        args[i].condFlag = &task_completed;
        args[i].condMutex = &condMutex;
        args[i].cond = &task_available;
        args[i].nextProductIndex = &productNext;
        args[i].productsCount = &productsCount; // for testing
        args[i].threadNum = thread_ids[i];
        args[i].teamNum = (int)type;

        ThreadArgs *p = malloc(sizeof *p);
        *p = args[i];

        // thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_function, p);
    }

    sleep(2); // wait so all threads are properly initlized

    while (1)
    {
        Notifier notifier;
        int recived;
        int notifierLenghth = sizeof(Notifier) - sizeof(long);

        if ((recived = msgrcv(qid, &notifier, notifierLenghth, type, IPC_NOWAIT)) != -1)
        {
            printf("here -----------------------------\n");

            productNext = notifier.index;
            pthread_mutex_t productMutex = products[productNext].productMutex;

            pthread_mutex_lock(&productMutex);

            productsCount = products[productNext].initialAmountOnShelves - products[productNext].currentAmountOnShelves;

            // printf("on shelve %d %d %d \n", products[next].initialAmountOnShelves, args.teamNum, args.threadNum);

            if (products[productNext].amountInStock >= productsCount)
            {

                products[productNext].amountInStock -= productsCount;

                pthread_mutex_lock(totalInStockmutex);

                *totalInStock -= productsCount;

                pthread_mutex_unlock(totalInStockmutex);
            }
            else
            {

                productsCount = products[productNext].amountInStock;

                products[productNext].amountInStock -= products[productNext].amountInStock;

                pthread_mutex_lock(totalInStockmutex);

                *totalInStock -= productsCount;

                if (*totalInStock == 0)
                {
                    kill(getppid(), SIGUSR1);
                    // pthread_mutex_unlock(totalInStockmutex);
                }

                pthread_mutex_unlock(totalInStockmutex);
            }

            // printf("on shelve after %d %d %d \n", products[next].initialAmountOnShelves, args.teamNum, args.threadNum);

            pthread_mutex_unlock(&productMutex);

            sleep((productsCount / 20) + 1); // simulate work

            pthread_mutex_lock(&condMutex);

            // Signal all threads that a task is available
            task_completed = 1;
            pthread_cond_broadcast(&task_available);

            pthread_mutex_unlock(&condMutex);

            pthread_barrier_wait(&barrier); // wait on barrier until all threads are done

            pthread_mutex_lock(&productMutex);

            products[productNext].underThreshold = 0;

            pthread_mutex_unlock(&productMutex);

            pthread_mutex_lock(&condMutex);
            /*reset cond flag so employees can wait for it.

            note that we should implemnet some sleep right after the barrier for the employees so we could assure that it has been reset*/
            task_completed = 0;
            pthread_mutex_unlock(&condMutex);
        }
    }
}

void *thread_function(void *arg)
{

    // TODO: check if the shelf you want to notify about has stock in customer

    ThreadArgs args = *(ThreadArgs *)arg;

    printf("thread cond num %d \n", *args.condFlag);

    while (1)
    {
        sleep(2); // wait a bit to make sure that the flag had been reset by manager

        pthread_mutex_lock(args.condMutex);

        if (!*args.condFlag)
        {
            printf(" flag val %d\n", *args.condFlag);
            pthread_cond_wait(args.cond, args.condMutex);
        }

        pthread_mutex_unlock(args.condMutex);

        printf("woken up thread team: %d I'm thread: %d\n", args.teamNum, args.threadNum);

        int next = *args.nextProductIndex;
        pthread_mutex_t productMutex = products[next].productMutex;
        int numberToShelf = (*args.productsCount) / NUM_EMPLOYEES_PER_TEAM;

        // printf("to be shelved by %d %d %d \n", numberToShelf, args.teamNum, args.threadNum);

        pthread_mutex_lock(&productMutex);

        printf("on shelve %d %d %d \n", products[next].initialAmountOnShelves, args.teamNum, args.threadNum);

        products[next].initialAmountOnShelves += numberToShelf;

        printf("on shelve after %d %d %d \n", products[next].initialAmountOnShelves, args.teamNum, args.threadNum);

        sleep((numberToShelf / 10) + 1); // simulate work

        pthread_mutex_unlock(&productMutex);

        pthread_barrier_wait(args.barrier);
    }

    free(arg);

    return NULL;
}
