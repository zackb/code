package net.jeedup.sql.impl;

import net.jeedup.sql.Sql;

import javax.sql.DataSource;
import java.io.Closeable;
import java.sql.Connection;
import java.sql.SQLException;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Manage DataSource level and expose underlying connection safely.
 *
 * Created by zack on 9/12/18.
 */
public class SqlDataSource implements Sql {

    private static final Logger log = Logger.getLogger(SqlDataSource.class.getSimpleName());

    private final DataSource ds;

    public SqlDataSource(DataSource dataSource) {
        this.ds = dataSource;
    }

    private void withConnection(ConnectionConsumer consumer) throws SQLException {
        try (Connection raw = ds.getConnection()) {
            try (SqlConnection connection = new SqlConnection(raw)) {
                consumer.connection(connection);
            }
        }
    }

    private <T> T withConnectionResult(ConnectionWithResultConsumer<T> consumer) throws SQLException  {
        try (Connection raw = ds.getConnection()) {
            try (SqlConnection connection = new SqlConnection(raw)) {
                return consumer.connection(connection);
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int execute(String sql) throws SQLException {
        return withConnectionResult(c -> c.execute(sql));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int execute(String sql, List<Object> params) throws SQLException {
        return withConnectionResult(c -> c.execute(sql, params));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int execute(String sql, Map<String, ?> params) throws SQLException {
        return withConnectionResult(c -> c.execute(sql, params));
    }

    @Override
    public Object executeInsert(String sql, List<Object> params) throws SQLException {
        return withConnectionResult(c -> c.executeInsert(sql, params));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void executeQuery(String sql) throws SQLException {
        withConnection(c -> c.executeQuery(sql));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void eachRow(String sql, RowConsumer consumer) throws SQLException {
        withConnection(c -> c.eachRow(sql, consumer));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public List<Row> rows(String sql) throws SQLException {
        return withConnectionResult(c -> c.rows(sql));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public Row firstRow(String sql) throws SQLException {
        return withConnectionResult(c -> c.firstRow(sql));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public long withTransaction(TransactionConsumer consumer) throws SQLException {
        return withConnectionResult(c -> c.withTransaction(consumer));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public long withTransaction(String... statements) throws SQLException {
        return withConnectionResult(c -> c.withTransaction(statements));
    }

    @FunctionalInterface
    interface ConnectionConsumer {
        void connection(SqlConnection connection) throws SQLException;
    }

    @FunctionalInterface
    interface ConnectionWithResultConsumer<T> {
        T connection(SqlConnection connection) throws SQLException;
    }

    /**
     * Close the connection datasource and free any resources
     */
    @Override
    public void close() {
        if (ds != null) {
            try {
                if (ds instanceof Closeable) {
                    ((Closeable) ds).close();
                } else if (ds instanceof AutoCloseable) {
                    ((AutoCloseable) ds).close();
                }
            } catch (Throwable t) {
                log.log(Level.SEVERE, "Failed closing datsource", t);
            }
        }
    }
}
