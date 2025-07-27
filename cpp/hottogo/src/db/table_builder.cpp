#include "db/table_builder.h"
#include "db/database.h"
#include "db/statement.h"
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
