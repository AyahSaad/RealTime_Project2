#include "fileReaders.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <pthread.h>

int PRODUCT_COUNT;
int NUM_SHELVING_TEAMS = 0;
int NUM_EMPLOYEES_PER_TEAM;
int SHELF_THRESHOLD;
int MAX_SIMULATION_TIME;
int CUSTOMER_ARRIVAL_RATE_LOWER;
int CUSTOMER_ARRIVAL_RATE_UPPER;
int CUSTOMER_SHOPPING_TIME_LOWER;
int CUSTOMER_SHOPPING_TIME_UPPER;

Product *products;


key_t Productskey;


pthread_mutexattr_t mutex_shared_attr;

void cleanupSharedMemory()
{
    if (shmdt(products) == -1)
    {
        printf("shmdt failed");
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    if (shmctl(shmget(Productskey, sizeof(Product) * PRODUCT_COUNT, IPC_CREAT | 0666), IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
}

int readProductsFile(const char *filename, int *totalInStock)
{
    Productskey = ftok("products.txt", 'R');

    int shmid;

    shmid = shmget(Productskey, sizeof(Product) * PRODUCT_COUNT, IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    Product *p = shmat(shmid, NULL, 0);
    if (p == (Product *)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    products = p;


    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        perror("Error opening file");
        cleanupSharedMemory();
        exit(EXIT_FAILURE);
    }

    char line[500];
    int i = 0;
    while (fgets(line, sizeof(line), file) != NULL && i < PRODUCT_COUNT)
    {
        char *token = strtok(line, ",");
        if (token == NULL)
        {
            break;
        }

        strcpy(products[i].name, token);

        token = strtok(NULL, ",");
        if (token == NULL)
        {
            break;
        }

        products[i].initialAmountOnShelves = atoi(token);
        products[i].currentAmountOnShelves = atoi(token);

      

        token = strtok(NULL, ",");
        if (token == NULL)
        {
            break;
        }

        products[i].amountInStock = atoi(token);

        *totalInStock += products[i].amountInStock;

        pthread_mutexattr_t mutex_shared_attr;

        pthread_mutexattr_init(&mutex_shared_attr);
        pthread_mutexattr_setpshared(&mutex_shared_attr, PTHREAD_PROCESS_SHARED);

        pthread_mutex_init(&products[i].productMutex, &mutex_shared_attr);

        products[i].underThreshold = 0;

        i++;
    }

    fclose(file);
    printf("shm total  is  %d\n", *totalInStock);

    return shmid;
}

void readConfigurationFile(const char *filename)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        perror("Error opening file");
        cleanupSharedMemory();
        exit(EXIT_FAILURE);
    }

    char line[500];
    while (fgets(line, sizeof(line), file) != NULL)
    {

        char *token = strtok(line, ":");

        if (token == NULL)
        {
            break;
        }

        char variableName[500];
        strcpy(variableName, token);

        token = strtok(NULL, ":");

        if (token == NULL)
        {
            break;
        }

        int value = atoi(token);

        if (strcmp(variableName, "PRODUCT_COUNT") == 0)
        {
            PRODUCT_COUNT = value;
        }
        else if (strcmp(variableName, "NUM_SHELVING_TEAMS") == 0)
        {
            NUM_SHELVING_TEAMS = value;
        }
        else if (strcmp(variableName, "NUM_EMPLOYEES_PER_TEAM") == 0)
        {
            NUM_EMPLOYEES_PER_TEAM = value;
        }
        else if (strcmp(variableName, "SHELF_THRESHOLD") == 0)
        {
            SHELF_THRESHOLD = value;
        }
        else if (strcmp(variableName, "MAX_SIMULATION_TIME") == 0)
        {
            MAX_SIMULATION_TIME = value;
        }
        else if (strcmp(variableName, "CUSTOMER_ARRIVAL_RATE_LOWER") == 0)
        {
            CUSTOMER_ARRIVAL_RATE_LOWER = value;
        }
        else if (strcmp(variableName, "CUSTOMER_ARRIVAL_RATE_UPPER") == 0)
        {
            CUSTOMER_ARRIVAL_RATE_UPPER = value;
        }
        else if (strcmp(variableName, "CUSTOMER_SHOPPING_TIME_LOWER") == 0)
        {
            CUSTOMER_SHOPPING_TIME_LOWER = value;
        }
        else if (strcmp(variableName, "CUSTOMER_SHOPPING_TIME_UPPER") == 0)
        {
            CUSTOMER_SHOPPING_TIME_UPPER = value;
        }
    }

    fclose(file);
}
