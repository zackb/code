#include "db/database.h"
#include "db/statement.h"
#include "db/table_builder.h"
#include <memory>
#include <sqlite3.h>

Database::Database(const std::string dbPath) {
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        throw std::runtime_error(sqlite3_errmsg(db));
    }
}

void Database::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

std::unique_ptr<Statement> Database::prepare(const std::string& query) {
    return std::make_unique<Statement>(*this, query);
}

TableBuilder Database::createTable(const std::string& name) { return TableBuilder(*this, name); }

Database::~Database() { close(); }
