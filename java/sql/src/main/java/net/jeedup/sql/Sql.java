package net.jeedup.sql;

import java.sql.SQLException;
import java.sql.Timestamp;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Interface to SQL connectivity and query execution
 * Created by zack on 9/12/18.
 */
public interface Sql extends AutoCloseable {

    /**
     * Execute a SQL statement.
     * @param sql any class of SQL statement (select/insert/update/delete/ddl/etc)
     * @return number of rows affected
     * @throws SQLException on bad SQL
     */
    int execute(String sql) throws SQLException;

    /**
     * Execute a SQL statement with parameter values;
     * @param sql any class of SQL statement (select/insert/update/delete/ddl/etc) that has question marks
     * @param params object parameters in the sql
     * @throws SQLException on bad SQL
     */
    int execute(String sql, List<Object> params) throws SQLException;

    /**
     * Execute a SQL statement with named parameters.
     * IMPORTANT: The name of the parameter can only be used once.
     * @param sql any class of SQL statement (select/insert/update/delete/ddl/etc)
     * @param params hash of name / value
     * @throws SQLException on bad SQL
     */
    int execute(String sql, Map<String, ?> params) throws SQLException;

    Object executeInsert(String sql, List<Object> params) throws SQLException;

    /**
     * Execute a SQL read query
     * @param sql select statement
     * @throws SQLException on bad SQL
     */
    void executeQuery(String sql) throws SQLException;

    /**
     * Execute a SQL read query and iterate over the results
     * @param sql select statement
     * @param consumer to deliver each row of results
     * @throws SQLException on bad SQL
     */
    void eachRow(final String sql, final RowConsumer consumer) throws SQLException;

    /**
     * Execute a SQL read query and return all rows.
     * This loads all results into memory in one shot
     * @param sql select statement
     * @return all rows returned by the select statement
     * @throws SQLException on bad SQL
     */
    List<Row> rows(final String sql) throws SQLException;

    /**
     * Execute a SQL read query and return the first row of the result set.
     * @param sql select statement
     * @return the first row of results
     * @throws SQLException on bad SQL
     */
    Row firstRow(final String sql) throws SQLException;


    /**
     * Execute statements within a transaction.
     * @param consumer to receive a {@link Sql} instance to run statements in a transaction
     * @return number of rows affected
     * @throws SQLException
     */
    long withTransaction(TransactionConsumer consumer) throws SQLException;

    long withTransaction(String... statements) throws SQLException;

    @FunctionalInterface
    interface RowConsumer {
        void row(final Row row);
    }

    @FunctionalInterface
    interface TransactionConsumer {
        void tx(final Transaction transaction) throws SQLException;
    }

    // util class to hold a row of query response data
    class Row extends HashMap<String, Object> {
        public Row(int size) {
            super(size);
        }

        public String getString(String name) {
            return (String)get(name);
        }

        public Long getLong(String name) {
            return (Long)get(name);
        }

        public Integer getInt(String name) {
            return (Integer)get(name);
        }

        public Double getDouble(String name) {
            return (Double)get(name);
        }

        public Float getFloat(String name) {
            return (Float)get(name);
        }

        public Number getNumber(String name) {
            return (Number)get(name);
        }

        public Boolean getBool(String name) {
            return (Boolean)get(name);
        }

        public Timestamp getTimestamp(String name) {
            return (Timestamp)get(name);
        }
    }
}
