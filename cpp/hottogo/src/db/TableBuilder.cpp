#include "db/TableBuilder.h"
#include "db/Database.h"
#include "db/Statement.h"
#include <sstream>

void TableBuilder::execute() {
    std::ostringstream sql;
    sql << "CREATE TABLE ";
    if (ifNotExists_)
        sql << "IF NOT EXISTS ";
    sql << tableName_ << " (";

    for (size_t i = 0; i < columns_.size(); ++i) {
        if (i > 0)
            sql << ", ";
        sql << columns_[i];
    }
    sql << ");";

    auto stmt = db_.prepare(sql.str());
    stmt->step();
}
