#include "fileReaders.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int PRODUCT_COUNT;
int NUM_SHELVING_TEAMS;
int NUM_EMPLOYEES_PER_TEAM;
int SHELF_THRESHOLD;
int MAX_SIMULATION_TIME;
int CUSTOMER_ARRIVAL_RATE_LOWER;
int CUSTOMER_ARRIVAL_RATE_UPPER;
int CUSTOMER_SHOPPING_TIME_LOWER;
int CUSTOMER_SHOPPING_TIME_UPPER;

void readProductsFile(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        perror("Error opening file");
        return;
    }

    char line[500];
    while (fgets(line, sizeof(line), file) != NULL) {

        char *token = strtok(line, ",");

        if (token == NULL) {
            break;
        }

        char productName[500];
        strcpy(productName, token);

        token = strtok(NULL, ",");
        
        if (token == NULL) {
            break;
        }

        int amountOnShelves = atoi(token);

        token = strtok(NULL, ",");
        
        if (token == NULL) {
            break;
        }

        int amountInStock = atoi(token);
        printf("Product: %s, Amount on Shelves: %d, Amount in Stock: %d\n", productName, amountOnShelves, amountInStock);
       
    }

    fclose(file);
}

void readConfigFile(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        perror("Error opening file");
        return;
    }

    char line[500];
    while (fgets(line, sizeof(line), file) != NULL) {

        char *token = strtok(line, ":");

        if (token == NULL) {

            break;
        }

        char variableName[500];
        strcpy(variableName, token);

        token = strtok(NULL, ":");
        
        if (token == NULL) {

            break;
        }

        int value = atoi(token);

        if (strcmp(variableName, "PRODUCT_COUNT") == 0) {
            PRODUCT_COUNT = value;
        }
        else if (strcmp(variableName, "NUM_SHELVING_TEAMS") == 0) {
            NUM_SHELVING_TEAMS = value;
        }
        else if (strcmp(variableName, "NUM_EMPLOYEES_PER_TEAM") == 0) {
            NUM_EMPLOYEES_PER_TEAM = value;
        }
        else if (strcmp(variableName, "SHELF_THRESHOLD") == 0) {
            SHELF_THRESHOLD = value;
        }
        else if (strcmp(variableName, "MAX_SIMULATION_TIME") == 0) {
            MAX_SIMULATION_TIME = value;
        }
        else if (strcmp(variableName, "CUSTOMER_ARRIVAL_RATE_LOWER") == 0) {
            CUSTOMER_ARRIVAL_RATE_LOWER = value;
        }
        else if (strcmp(variableName, "CUSTOMER_ARRIVAL_RATE_UPPER") == 0) {
            CUSTOMER_ARRIVAL_RATE_UPPER = value;
        }
        else if (strcmp(variableName, "CUSTOMER_SHOPPING_TIME_LOWER") == 0) {
            CUSTOMER_SHOPPING_TIME_LOWER = value;
        }
        else if (strcmp(variableName, "CUSTOMER_SHOPPING_TIME_UPPER") == 0) {
            CUSTOMER_SHOPPING_TIME_UPPER = value;
        }
    }

    fclose(file);
    
    printf("PRODUCT_COUNT: %d\n", PRODUCT_COUNT);
    printf("NUM_SHELVING_TEAMS: %d\n", NUM_SHELVING_TEAMS);
    printf("NUM_EMPLOYEES_PER_TEAM: %d\n", NUM_EMPLOYEES_PER_TEAM);
    printf("SHELF_THRESHOLD: %d\n", SHELF_THRESHOLD);
    printf("MAX_SIMULATION_TIME: %d\n", MAX_SIMULATION_TIME);
    printf("CUSTOMER_ARRIVAL_RATE_LOWER: %d\n", CUSTOMER_ARRIVAL_RATE_LOWER);
    printf("CUSTOMER_ARRIVAL_RATE_UPPER: %d\n", CUSTOMER_ARRIVAL_RATE_UPPER);
    printf("CUSTOMER_SHOPPING_TIME_LOWER: %d\n", CUSTOMER_SHOPPING_TIME_LOWER);
    printf("CUSTOMER_SHOPPING_TIME_UPPER: %d\n", CUSTOMER_SHOPPING_TIME_UPPER);

}
