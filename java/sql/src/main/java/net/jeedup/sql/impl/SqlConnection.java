package net.jeedup.sql.impl;

import net.jeedup.sql.NamedParameterStatement;
import net.jeedup.sql.Sql;
import net.jeedup.sql.Transaction;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicLong;
import java.util.logging.Level;
import java.util.logging.Logger;

import static net.jeedup.common.util.Util.empty;

/**
 * Interface to SQL query execution
 * Created by zack on 9/12/18.
 */
public class SqlConnection implements Sql {

    private static final Logger log = Logger.getLogger(SqlConnection.class.getSimpleName());

    private final Connection connection;

    public SqlConnection(Connection connection) {
        this.connection = connection;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int execute(String sql) throws SQLException {
        log.info("Running sql: " + sql);
        try (PreparedStatement statement = connection.prepareStatement(sql)) {
            return statement.executeUpdate();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int execute(String sql, Map<String, ?> params) throws SQLException {
        log.info("Running sql: " + sql);
        try (NamedParameterStatement stmt = new NamedParameterStatement(connection, sql)) {

            for (String key : params.keySet())
                stmt.set(key, params.get(key));

            return stmt.executeUpdate();
        }
    }

    /**
     * Execute a SQL read query
     * @param sql select statement
     * @throws SQLException on bad SQL
     */
    @Override
    public void executeQuery(String sql) throws SQLException {
        log.info("Running sql: " + sql);
        try (PreparedStatement statement = connection.prepareStatement(sql)) {
            statement.executeQuery();
        }
    }

    /**
     * Execute a SQL read query and iterate over the results
     * @param sql select statement
     * @param consumer to deliver each row of results
     * @throws SQLException on bad SQL
     */
    @Override
    public void eachRow(final String sql, final RowConsumer consumer) throws SQLException {
        log.info("Running sql: " + sql);
        try (PreparedStatement statement = connection.prepareStatement(sql)) {
            try (ResultSet rs = statement.executeQuery()) {
                ResultSetMetaData md = rs.getMetaData();
                int cols = md.getColumnCount();
                while (rs.next()) {
                    Row row = new Row(cols);
                    for (int i = 1; i <= cols; i++) {
                        row.put(md.getColumnName(i), rs.getObject(i));
                    }
                    consumer.row(row);
                }
            }
        }
    }

    /**
     * Execute a SQL read query and return all rows.
     * This loads all results into memory in one shot
     * @param sql select statement
     * @return all rows returned by the select statement
     * @throws SQLException on bad SQL
     */
    @Override
    public List<Row> rows(final String sql) throws SQLException {
        List<Row> rows = new ArrayList<>();
        eachRow(sql, rows::add);
        return rows;
    }

    /**
     * Execute a SQL read query and return the first row of the result set.
     * @param sql select statement
     * @return the first row of results
     * @throws SQLException on bad SQL
     */
    @Override
    public Row firstRow(final String sql) throws SQLException {
        List<Row> rows = rows(sql);
        return rows.isEmpty() ? null : rows.get(0);
    }

    /**
     * Begin a database transaction and receive a sql connection to execute statements inside that transaction.
     * The transaction is started and committed automatically, or rolled back if an exception is thrown by the
     * transaction consumer.
     * @param consumer to receive a {@link Sql} instance to run statements in a transaction
     * @return number of rows effected in the transaction
     * @throws SQLException on communication failure or invalid SQL
     */
    @Override
    public long withTransaction(TransactionConsumer consumer) throws SQLException {
        Transaction transaction = null;
        try {
            transaction = new Transaction(connection);
            transaction.begin();
            consumer.tx(transaction);
            transaction.end();
        } catch (Throwable t) {
            if (transaction != null) {
                transaction.rollback();
            }
            throw t;
        } finally {
            close();
        }
        return transaction == null ? 0 : transaction.getUpdates();
    }

    /**
     * Run SQL statements inside of a transaction.
     * @param statements SQL to run in transaction
     * @return number of rows effected
     * @throws SQLException
     */
    @Override
    public long withTransaction(String... statements) throws SQLException {
        AtomicLong updated = new AtomicLong(0);
        boolean autoCommit = connection.getAutoCommit();
        PreparedStatement preparedStatement = null;
        try {
            connection.setAutoCommit(false);
            for (String stmt : statements) {

                // skip newlines, for convenience and SQL readability
                if (empty(stmt) || "\n".equals(stmt)) {
                    log.fine("Skipping empty statement: '" + stmt + "'");
                    continue;
                }

                log.info("Running SQL in transaction: '" + stmt + "'");
                preparedStatement = connection.prepareStatement(stmt);
                updated.addAndGet(preparedStatement.executeUpdate());
                preparedStatement.close();
            }
            connection.commit();
        } catch (SQLException e) {
            log.log(Level.SEVERE, "Failed executing transaction, rolling back", e);
            connection.rollback();
            if (preparedStatement != null && !preparedStatement.isClosed()) {
                preparedStatement.close();
            }
            throw e;
        } finally {
            connection.setAutoCommit(autoCommit);
        }
        return updated.get();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void close() throws SQLException {
        if (connection != null && !connection.isClosed()) {
            connection.close();
        }
    }
}
