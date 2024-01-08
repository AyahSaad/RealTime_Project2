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

    // Dynamically allocate an array to keep track of which products the customer has already picked
    int *productsPicked = malloc(sizeof(int) * PRODUCT_COUNT);
    if (productsPicked == NULL) {
        perror("Failed to allocate memory for productsPicked");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < PRODUCT_COUNT; i++) {
        productsPicked[i] = 0;
    }

    int shoppingTime = rand() % (CUSTOMER_SHOPPING_TIME_UPPER - CUSTOMER_SHOPPING_TIME_LOWER + 1) +
                       CUSTOMER_SHOPPING_TIME_LOWER;
    sleep(shoppingTime);

    int numProductsToPick = getRandomNumber(1, PRODUCT_COUNT);

    
    for (int i = 0; i < numProductsToPick; i++) {
      
        int productIndex;
        do {
            productIndex = getRandomNumber(0, PRODUCT_COUNT - 1);
        } while (productsPicked[productIndex] == 1);

        pthread_mutex_lock(&products[productIndex].productMutex);

        int maxQuantity = products[productIndex].initialAmountOnShelves;
        int quantity = getRandomNumber(1, maxQuantity);
        printf("Customer %d picked %d units of %s.\n", customerID, quantity, products[productIndex].name);

        products[productIndex].initialAmountOnShelves -= quantity;
        productsPicked[productIndex] = 1;

        pthread_mutex_unlock(&products[productIndex].productMutex);
    }

    
    free(productsPicked);

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
