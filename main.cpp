#include <iostream>
#include "src/database/connection.h"

int main() {
    Database::initialise("../data/db.db");

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
