#include "fileReaders.h"

int main() {
    const char *configFilePath = "argument.txt";
    const char *productFilePath = "products.txt";

    readConfigFile(configFilePath);
    readProductsFile(productFilePath);

    return 0;
}
