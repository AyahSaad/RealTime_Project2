#include "customer.h"
#include "fileReaders.h"
#include "team.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/prctl.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int getRandomNumber(int lower, int upper)
{
    return rand() % (upper - lower + 1) + lower;
}


void simulateCustomerShopping(int customerID, int qid)
{
    srand(time(NULL) ^ (getpid() << 16)); // to ensure that the seed is actually random for each customer process

    printf("Customer %d started shopping.\n", customerID);

    int shoppingTime = getRandomNumber(CUSTOMER_SHOPPING_TIME_LOWER, CUSTOMER_SHOPPING_TIME_UPPER);

    int startingProductIndex = getRandomNumber(0, PRODUCT_COUNT - 1);
    int numProductsToPick = getRandomNumber(1, PRODUCT_COUNT);

    for (int i = startingProductIndex; i < numProductsToPick + startingProductIndex; i++)
    {
        int productIndex = (i) % PRODUCT_COUNT;

        pthread_mutex_lock(&products[productIndex].productMutex);

        int maxQuantity = products[productIndex].currentAmountOnShelves;

        if (maxQuantity > 0)
        {
            int quantity = getRandomNumber(1, ((maxQuantity / 15) + 1));

            printf("Customer %d picked %d units of %s.\n", customerID, quantity, products[productIndex].name);

            products[productIndex].currentAmountOnShelves -= quantity;

           
            if (products[productIndex].currentAmountOnShelves <= SHELF_THRESHOLD && products[productIndex].amountInStock > 0 && products[productIndex].underThreshold != 1)
            {
                products[productIndex].underThreshold = 1;


                Notifier notifier;
                int notifierLenghth = sizeof(notifier) - sizeof(long);

                int chooseTeam = getRandomNumber(1, NUM_SHELVING_TEAMS);

                long type = (long)chooseTeam;

                notifier.index = productIndex;
                notifier.mtype = type;

                if (msgsnd(qid, &notifier, notifierLenghth, 0) == -1) // send message
                {
                    perror("msgsnd");
                    exit(EXIT_FAILURE);
                }

                printf("sent to team %d for shelf %d \n", chooseTeam, productIndex);
            }
        }
        else
        {
            printf("Customer %d wanted to pick %s, but it's out of stock.\n", customerID, products[productIndex].name);
        }

        sleep(shoppingTime);

        pthread_mutex_unlock(&products[productIndex].productMutex);
    }

    printf("Customer %d finished shopping.\n", customerID);
}

void simulateCustomerArrival(int qid)
{
    int arrivalPid = getpid();
    srand(time(NULL) ^ (getpid() << 16));
    int randArrival = getRandomNumber(CUSTOMER_ARRIVAL_RATE_LOWER, CUSTOMER_ARRIVAL_RATE_UPPER);
    printf("Arrival time: %d\n", randArrival);

    while (1)
    {
        if (getpid() == arrivalPid)
        {

            sleep(randArrival);

            int forkAcustomer = fork();

            if (forkAcustomer == 0)
            {
                char *name = "CustomerProc";
                prctl(PR_SET_NAME, (unsigned long)name);

                printf("I am customer %d \n", getpid());
                simulateCustomerShopping(getpid(), qid);
            }
        }
    }
}
