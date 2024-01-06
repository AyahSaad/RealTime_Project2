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

void readConfigFile(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        perror("Error opening file");
        return;
    }

    char line[500];
    // Loop through each line in the file
    while (fgets(line, sizeof(line), file) != NULL) {

        char *token = strtok(line, ":");

        if (token == NULL) {
            // Check for the end of file
            break;
        }

        char variableName[500];
        strcpy(variableName, token);

        token = strtok(NULL, ":");

        if (token == NULL) {
            // Check for the end of file
            break;
        }

        char valString[50];
        strcpy(valString, token);

        int value;

        value = atoi(valString);

        // Assign the value to the corresponding global variable
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
}
