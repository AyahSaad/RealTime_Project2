#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include "customer.h"
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "fileReaders.h"
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/prctl.h>
#include <string.h>
#include "team.h"

int mainParent;
int totalInStockShmid;

int main()
{
    mainParent = getpid();

    key_t totalInStockKey = ftok(".", 's'); // Generate a key for cashiers left int shm

    if ((totalInStockShmid = shmget(totalInStockKey, sizeof(int), IPC_CREAT | 0666)) < 0)
    { // Create a shared memory segment for total in stock int shm
        perror("shmget");
        exit(1);
    }

    int *totalInStock = (int *)shmat(totalInStockShmid, 0, 0); // attach to main process memory space

    printf("shm customer  is  %d\n", *totalInStock);

    if (totalInStock == (int *)-1)
    {
        perror("shmat");
        exit(1);
    }

    pthread_mutex_t totalInStockMutex;

    pthread_mutexattr_t mutex_shared_attr;

    pthread_mutexattr_init(&mutex_shared_attr);
    pthread_mutexattr_setpshared(&mutex_shared_attr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&totalInStockMutex, &mutex_shared_attr);

    // pthread_mutex_lock(&totalInStockMutex);
    // *totalInStock = 3; // set initial value of total in stock to 0
    // printf("shm customer  is  %d\n", *totalInStock);
    // pthread_mutex_unlock(&totalInStockMutex);

    char *configFilePath = "arguments.txt";
    char *productFilePath = "products.txt";

    readConfigurationFile(configFilePath);
    readProductsFile(productFilePath, totalInStock);

    key_t Queuekey = ftok(".", 'q');              // Generate a key for queue
    int qid = msgget(Queuekey, IPC_CREAT | 0666); // Create a queue and get the queue id
    printf("this is qid %d\n", qid);
    if (qid == -1)
    {
        perror("msgget");
        exit(1);
    }

    int pid;

    int hasEntered = 0;
    int order = 0;
    long teamType = 1L;

    for (int i = 0; i < NUM_SHELVING_TEAMS; i++, teamType++)
    {
        // only main parent function can fork so we can get n processes not 2^n
        if (getpid() == mainParent || pid != 0)
        {

            pid = fork();
            order++;

            if (pid == -1)
            {
                printf("fork failure ... getting out\n");
                perror("fork");
            }
        }

        if (pid == 0 && hasEntered != 1) // only new team process will enter
        {
            char *name = "TeamProc";
            prctl(PR_SET_NAME, (unsigned long)name);

            // sleep(20);
            hasEntered = 1;
            printf("I am team %d and my type is %ld \n", i, teamType);

            teamFunc(teamType, qid, totalInStock, &totalInStockMutex);
        }
    }

    // sleep(30);
    if (getpid() == mainParent)
    {

        int forkArrivals = fork();

        if (forkArrivals == -1)
        {
            printf("fork failure... getting out\n");
            perror("fork");
        }

        if (forkArrivals == 0)
        {
            char *name = "ArrivalsProc";
            prctl(PR_SET_NAME, (unsigned long)name);
            simulateCustomerArrival(qid);
        }
        else
        {
            int status;
            waitpid(forkArrivals, &status, 0);

            // TODO: openGL and Sig Alaram for termination

            // glutInit(&argc, argv);
            // glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
            // glutInitWindowSize(windowWidth, windowHeight);
            // glutCreateWindow("Cash Registers");

            // glutDisplayFunc(display);
            // glutReshapeFunc(reshape);

            // glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set clear color to black

            // glutMainLoop();
        }
    }

    // Cleanup shared memory
    // cleanupSharedMemory();

    return 0;
}