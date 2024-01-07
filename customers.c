#include "fileReaders.h"
#include "customer.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

int getRandomNumber(int lower, int upper) {
    return rand() % (upper - lower + 1) + lower;
}

void simulateCustomerShopping(int customerID) {
    printf("Customer %d started shopping.\n", customerID);

    int shoppingTime = rand() % (CUSTOMER_SHOPPING_TIME_UPPER - CUSTOMER_SHOPPING_TIME_LOWER + 1) +
                       CUSTOMER_SHOPPING_TIME_LOWER;

    sleep(shoppingTime);

    // Simulate choosing items and quantities
    for (int i = 0; i < PRODUCT_COUNT; i++) {
        pthread_mutex_lock(&products[i].productMutex);  
        int quantity = getRandomNumber(1, products[i].initialAmountOnShelves);
        printf("Customer %d picked %d units of %s.\n", customerID, quantity, products[i].name);
        products[i].initialAmountOnShelves -= quantity;
        pthread_mutex_unlock(&products[i].productMutex);  
    }

    printf("Customer %d finished shopping.\n", customerID);
}

void simulateCustomerArrival(int *shelfQuantities) {
    
    srand(time(NULL) ^ (getpid() << 16));

    while (1) {
        int randArrival = getRandomNumber(CUSTOMER_ARRIVAL_RATE_LOWER, CUSTOMER_ARRIVAL_RATE_UPPER);
        printf("Arrival time: %d\n", randArrival);

        sleep(randArrival);
        
        int forkAcustomer = fork();

        if (forkAcustomer == 0) {
            printf("I am customer %d \n", getpid());
            simulateCustomerShopping(getpid());
            exit(EXIT_SUCCESS);
        }
    }
}
