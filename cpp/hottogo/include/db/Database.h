#pragma once

#include <sqlite3.h>
#include <string>

class Statement;
class TableBuilder;

class Database {

public:
    Database(std::string dbPath);
    ~Database();

    std::unique_ptr<Statement> prepare(const std::string& query);
    TableBuilder createTable(const std::string& name);

    void close();

private:
    friend class Statement;
    sqlite3* db;
    sqlite3* getHandle() const { return db; }
};
