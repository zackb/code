#include "db/statement.h"

Statement::Statement(Database& db, const std::string& query) {
    if (sqlite3_prepare_v2(db.getHandle(), query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error("Failed to prepare statement");
}

Statement::~Statement() { sqlite3_finalize(stmt); }

void Statement::bind(int index, int value) { sqlite3_bind_int(stmt, index, value); }

void Statement::bind(int index, const std::string& value) {
    sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
}

bool Statement::step() { return sqlite3_step(stmt) == SQLITE_ROW; }

bool Statement::execute() {
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        return true;
    } else if (rc == SQLITE_ROW) {
        // Possibly misuse — the statement returned rows
        throw std::runtime_error("Statement returned rows; use step() instead");
    } else {
        throw std::runtime_error(sqlite3_errstr(rc));
    }
}

int Statement::getInt(int column) { return sqlite3_column_int(stmt, column); }

std::string Statement::getText(int column) { return reinterpret_cast<const char*>(sqlite3_column_text(stmt, column)); }

void Statement::reset() {
    sqlite3_reset(stmt);
    sqlite3_clear_bindings(stmt);
}
