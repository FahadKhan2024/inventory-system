#ifndef INVENTORY_SYSTEM_CONNECTION_H
#define INVENTORY_SYSTEM_CONNECTION_H

#include <sqlite3.h>
#include <string>

class Database {
public:
    static void initialise(const std::string& dbPath);
    static sqlite3* getConnection();
    static void runMigrations();
    static int close();
    static bool isInitialised();
    ~Database();


private:
    static sqlite3* db;
    static bool initialised;
};

#endif //INVENTORY_SYSTEM_CONNECTION_H
