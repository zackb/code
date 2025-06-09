#pragma once

#include "Database.h"
#include <sqlite3.h>
#include <string>

class RowView {
public:
    explicit RowView(sqlite3_stmt* stmt) : stmt(stmt) {}

    int getInt(int col) const { return sqlite3_column_int(stmt, col); }

    std::string getText(int col) const {
        const unsigned char* text = sqlite3_column_text(stmt, col);
        return text ? reinterpret_cast<const char*>(text) : "";
    }

private:
    sqlite3_stmt* stmt;
};

class StatementIterator {
public:
    StatementIterator(sqlite3_stmt* stmt, bool isEnd = false) : stmt(stmt), isEnd(isEnd) {
        if (!isEnd && sqlite3_step(stmt) != SQLITE_ROW)
            isEnd = true;
    }

    RowView operator*() const { return RowView(stmt); }

    StatementIterator& operator++() {
        if (sqlite3_step(stmt) != SQLITE_ROW)
            isEnd = true;
        return *this;
    }

    bool operator!=(const StatementIterator& other) const { return isEnd != other.isEnd || stmt != other.stmt; }

private:
    sqlite3_stmt* stmt;
    bool isEnd;
};

class Statement {
public:
    Statement(Database& db, const std::string& query);
    ~Statement();

    void bind(int index, int value);
    void bind(int index, const std::string& value);
    bool step();
    int getInt(int column);
    std::string getText(int column);
    void reset();

    StatementIterator begin() {
        sqlite3_reset(stmt); // ensure we start from the beginning
        return StatementIterator(stmt, false);
    }

    StatementIterator end() { return StatementIterator(stmt, true); }

private:
    sqlite3_stmt* stmt;
};
