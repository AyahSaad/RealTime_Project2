#ifndef FILEREADERS_H
#define FILEREADERS_H
#include <pthread.h>

typedef struct
{
    char name[100];
    int initialAmountOnShelves;
    int currentAmountOnShelves;
    int amountInStock;
    int underThreshold;           // flag to indicate that it's under threshold
    pthread_mutex_t productMutex; // Mutex for each product

} Product;

extern int PRODUCT_COUNT;
extern int NUM_SHELVING_TEAMS;
extern int NUM_EMPLOYEES_PER_TEAM;
extern int SHELF_THRESHOLD;
extern int MAX_SIMULATION_TIME;
extern int CUSTOMER_ARRIVAL_RATE_LOWER;
extern int CUSTOMER_ARRIVAL_RATE_UPPER;
extern int CUSTOMER_SHOPPING_TIME_LOWER;
extern int CUSTOMER_SHOPPING_TIME_UPPER;

extern Product *products;

extern pthread_mutexattr_t mutex_shared_attr;

int readProductsFile(const char *filename, int *totalInStock);
void readConfigurationFile(const char *filename);
void cleanupSharedMemory();

#endif