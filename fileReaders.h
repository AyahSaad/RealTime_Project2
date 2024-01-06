#ifndef FILE_READERS_H
#define FILE_READERS_H

extern int PRODUCT_COUNT;
extern int NUM_SHELVING_TEAMS;
extern int NUM_EMPLOYEES_PER_TEAM;
extern int SHELF_THRESHOLD;
extern int MAX_SIMULATION_TIME;
extern int CUSTOMER_ARRIVAL_RATE_LOWER;
extern int CUSTOMER_ARRIVAL_RATE_UPPER;
extern int CUSTOMER_SHOPPING_TIME_LOWER;
extern int CUSTOMER_SHOPPING_TIME_UPPER;

void readConfigFile(const char *filename);
void readProductsFile(const char *filename);

#endif
