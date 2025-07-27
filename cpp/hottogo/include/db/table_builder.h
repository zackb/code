#pragma once

#include "db/database.h"
#include <string>
#include <vector>

class TableBuilder {
public:
    TableBuilder(Database& db, const std::string& name) : db_(db), tableName_(name), ifNotExists_(false) {}

    TableBuilder& ifNotExists() {
        ifNotExists_ = true;
        return *this;
    }

    TableBuilder& column(const std::string& name, const std::string& typeSpec) {
        columns_.emplace_back(name + " " + typeSpec);
        return *this;
    }

    void execute();

private:
    Database& db_;
    std::string tableName_;
    bool ifNotExists_;
    std::vector<std::string> columns_;
};
