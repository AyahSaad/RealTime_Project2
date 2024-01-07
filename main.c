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

int main() {
    const char *configFilePath = "argument.txt";
    const char *productFilePath = "products.txt";

    readConfigurationFile(configFilePath);
    readProductsFile(productFilePath);

    int shelfQuantities[PRODUCT_COUNT];
    for (int i = 0; i < PRODUCT_COUNT; ++i) {
        shelfQuantities[i] = products[i].initialAmountOnShelves;
    }

    simulateCustomerArrival(shelfQuantities);

    // Cleanup shared memory
    cleanupSharedMemory();

    return 0;
}