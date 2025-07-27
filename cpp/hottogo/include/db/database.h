#pragma once

#include <memory>
#include <sqlite3.h>
#include <string>

class Statement;
class TableBuilder;

class Database {

public:
    explicit Database() = default;
    explicit Database(std::string dbPath);
    ~Database();

    std::unique_ptr<Statement> prepare(const std::string& query);
    TableBuilder createTable(const std::string& name);

    void open(const std::string& dbPath);
    void close();

    // prevent copying and moving
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    Database(Database&&) = delete;
    Database& operator=(Database&&) = delete;

private:
    friend class Statement;
    sqlite3* db = nullptr;
    sqlite3* getHandle() const { return db; }
};
