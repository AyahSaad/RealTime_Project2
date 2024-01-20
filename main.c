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
#include "openGLfunctions.h"
#include <GL/glut.h>
#include "team.h"

int mainParent;

int totalInStockShmid;
int productsShmid;
int qid;

void alarm_handler(int signo)
{
    if (signo == SIGALRM)
    {
        printf("Time's up! Terminating the process.\n");
        msgctl(qid, IPC_RMID, NULL);
        shmctl(totalInStockShmid, IPC_RMID, NULL);
        shmctl(productsShmid, IPC_RMID, NULL);
        shmctl(managerCountShmid, IPC_RMID, NULL);

        kill(0, SIGKILL);
    }
}

void sigusr1_handler(int signo)
{

    if (signo == SIGUSR1)
    {
        printf("Out of Stock! Terminating the process.\n");
        msgctl(qid, IPC_RMID, NULL);
        shmctl(totalInStockShmid, IPC_RMID, NULL);
        shmctl(productsShmid, IPC_RMID, NULL);
        shmctl(managerCountShmid, IPC_RMID, NULL);

        kill(0, SIGKILL);
    }
}

int main(int argc, char **argv)
{
    if (signal(SIGUSR1, sigusr1_handler) == SIG_ERR)
    {
        perror("Error setting up signal handler");
        return EXIT_FAILURE;
    }

    if (signal(SIGALRM, alarm_handler) == SIG_ERR)
    {
        perror("Error setting up signal handler");
        return EXIT_FAILURE;
    }

    mainParent = getpid();

    key_t totalInStockKey = ftok(".", 's'); // Generate a key for cashiers left int shm

    if ((totalInStockShmid = shmget(totalInStockKey, sizeof(int), IPC_CREAT | 0666)) < 0)
    { // Create a shared memory segment for total in stock int shm
        perror("shmget");
        exit(1);
    }

    int *totalInStock = (int *)shmat(totalInStockShmid, 0, 0); // attach to main process memory space

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

    char *configFilePath = "arguments.txt";
    char *productFilePath = "products.txt";

    readConfigurationFile(configFilePath);
    alarm(60 * MAX_SIMULATION_TIME);

    productsShmid = readProductsFile(productFilePath, totalInStock);

    key_t Queuekey = ftok(".", 'q');          // Generate a key for queue
    qid = msgget(Queuekey, IPC_CREAT | 0666); // Create a queue and get the queue id
    printf("this is qid %d\n", qid);
    if (qid == -1)
    {
        perror("msgget");
        exit(1);
    }

    initManagerInStock();

    printf("managers in stock in main is %d\n", *managersInStock);

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
            glutInit(&argc, argv);
            glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
            glutCreateWindow("SuperMarket Simulation");

            glutDisplayFunc(display);
            glutReshapeFunc(reshape);

            // Set the clear color to white
            glClearColor(1.0, 1.0, 1.0, 1.0);

            glutMainLoop();
        }
    }

    return 0;
}