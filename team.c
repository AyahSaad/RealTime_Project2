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
#include <sys/shm.h>

int *managersInStock;

int managerCountShmid;

pthread_mutex_t managersInStockMutex;

pthread_mutexattr_t mutex_shared_attrr;

void initManagerInStock()
{

    key_t managersKey = ftok("products.txt", 'g'); // Generate a key for cashiers left int shm

    if ((managerCountShmid = shmget(managersKey, sizeof(int), IPC_CREAT | 0666)) < 0)
    { // Create a shared memory segment for total in stock int shm
        perror("shmget");
        exit(1);
    }

    managersInStock = (int *)shmat(managerCountShmid, 0, 0); // attach to main process memory space

    if (managersInStock == (int *)-1)
    {
        perror("shmat");
        exit(1);
    }

    pthread_mutexattr_init(&mutex_shared_attrr);
    pthread_mutexattr_setpshared(&mutex_shared_attrr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&managersInStockMutex, &mutex_shared_attrr);
    *managersInStock = 0;
    printf("managers now is %d\n", *managersInStock);
}

void teamFunc(long type, int qid, int *totalInStock, pthread_mutex_t *totalInStockmutex)
{
    printf("------------------- %d \n ", *managersInStock);

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
        args[i].barrier = &barrier;

        ThreadArgs *p = malloc(sizeof *p);
        *p = args[i];

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
            sleep(2);

            printf("here -----------------------------team %ld\n", notifier.mtype);

            productNext = notifier.index;

            pthread_mutex_lock(&managersInStockMutex);

            *managersInStock += 1;

            pthread_mutex_unlock(&managersInStockMutex);

            pthread_mutex_lock(&products[productNext].productMutex);

            printf("got product mutex %d\n", productNext);

            productsCount = products[productNext].initialAmountOnShelves - products[productNext].currentAmountOnShelves;

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
                }

                pthread_mutex_unlock(totalInStockmutex);
            }

            pthread_mutex_unlock(&products[productNext].productMutex);

            sleep((productsCount / 20) + 1); // simulate work

            pthread_mutex_lock(&managersInStockMutex);

            *managersInStock -= 1;

            pthread_mutex_unlock(&managersInStockMutex);

            // sleep(2);

            pthread_mutex_lock(&condMutex);

            printf("Manager of team %ld awake now  ----------\n", type);

            // Signal all threads that a task is available
            task_completed = 1;
            pthread_cond_broadcast(&task_available);

            pthread_mutex_unlock(&condMutex);

            pthread_barrier_wait(&barrier); // wait on barrier until all threads are done

            pthread_mutex_lock(&condMutex);
            /*reset cond flag so employees can wait for it.

            note that we should implemnet some sleep right after the barrier for the employees so we could assure that it has been reset*/
            task_completed = 0;
            pthread_mutex_unlock(&condMutex);

            printf("team %ld -------- finished\n", notifier.mtype);

            pthread_mutex_lock(&products[productNext].productMutex);

            products[productNext].underThreshold = 0;

            pthread_mutex_unlock(&products[productNext].productMutex);
        }
    }
}

void *thread_function(void *arg)
{

    ThreadArgs args = *(ThreadArgs *)arg;

    printf("thread cond num %d \n", *args.condFlag);

    while (1)
    {
        sleep(2); // wait a bit to make sure that the flag had been reset by manager

        pthread_mutex_lock(args.condMutex);

        if (!*args.condFlag)
        {
            printf(" flag val %d\n", *args.condFlag);
            printf("thread %d is reset from team %d\n", args.threadNum, args.teamNum);
            pthread_cond_wait(args.cond, args.condMutex);
        }

        pthread_mutex_unlock(args.condMutex);

        printf("woken up thread team: %d I'm thread: %d\n", args.teamNum, args.threadNum);

        int next = *args.nextProductIndex;

        int numberToShelf = ((*args.productsCount) / NUM_EMPLOYEES_PER_TEAM) + 1;

        printf("to be shelved by %d %d %d \n", numberToShelf, args.teamNum, args.threadNum);

        pthread_mutex_lock(&products[next].productMutex);

        printf("on shelve %d %d %d \n", products[next].currentAmountOnShelves, args.teamNum, args.threadNum);

        products[next].currentAmountOnShelves += numberToShelf;

        printf("on shelve after %d %d %d \n", products[next].currentAmountOnShelves, args.teamNum, args.threadNum);

        sleep((numberToShelf / 10) + 1); // simulate work

        printf("thread %d finished \n", args.threadNum);

        pthread_mutex_unlock(&products[next].productMutex);
        printf("reached after \n");

        pthread_barrier_wait(args.barrier);
        printf("reached after barrier \n");
    }

    free(arg);

    return NULL;
}
