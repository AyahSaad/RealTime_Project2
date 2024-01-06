#include "fileReaders.h"
#include <stdio.h>

int main() {
    // Replace "config.txt" with the actual path to your configuration file
    const char *configFileName = "argument.txt";

    // Read the configuration file
    readConfigFile(configFileName);

    // Print the values to verify
    printf("PRODUCT_COUNT: %d\n", PRODUCT_COUNT);
    printf("NUM_SHELVING_TEAMS: %d\n", NUM_SHELVING_TEAMS);
    printf("NUM_EMPLOYEES_PER_TEAM: %d\n", NUM_EMPLOYEES_PER_TEAM);
    printf("SHELF_THRESHOLD: %d\n", SHELF_THRESHOLD);
    printf("MAX_SIMULATION_TIME: %d\n", MAX_SIMULATION_TIME);
    printf("CUSTOMER_ARRIVAL_RATE_LOWER: %d\n", CUSTOMER_ARRIVAL_RATE_LOWER);
    printf("CUSTOMER_ARRIVAL_RATE_UPPER: %d\n", CUSTOMER_ARRIVAL_RATE_UPPER);
    printf("CUSTOMER_SHOPPING_TIME_LOWER: %d\n", CUSTOMER_SHOPPING_TIME_LOWER);
    printf("CUSTOMER_SHOPPING_TIME_UPPER: %d\n", CUSTOMER_SHOPPING_TIME_UPPER);

    return 0;
}
