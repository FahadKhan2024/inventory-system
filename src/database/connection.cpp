//
// Created by fkkha on 31/08/2025.
//
#include "connection.h"
#include <iostream>
#include <fstream>
#include <stdexcept>


void Database::initialise(const std::string &dbPath) {
    if (initialised) return;

    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        throw std::runtime_error("Cannot open database" + std::string(sqlite3_errmsg(db)));
    }

    runMigrations();
    initialised = true;

    std::cout << "Database initialised successfully" << std::endl;
}

sqlite3* Database::getConnection() {
    return db;
}

bool Database::isInitialised() {
    return initialised;
}

void Database::runMigrations() {
    std::ifstream file("src/database/migrations/001_initial_schema.sql");

    if (!file.is_open()) {
        throw std::runtime_error("Cannot open the migration file");
    }

    std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    char* errMsg = nullptr;

    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::string error = "SQL error:" + std::string(errMsg);
        sqlite3_free(errMsg);
        throw std::runtime_error(error);
    }

    std::cout << "Migrations completed" << std::endl;
}

int Database::close() {
    if (!initialised) {
        std::cout << "Database already closed" << std::endl;
        return -1;
    }

    int result = sqlite3_close_v2(db);
    if (result == SQLITE_OK) {
        initialised = false;
        db = nullptr;
    }
    return result;
}

Database::~Database() {
    close();
}

