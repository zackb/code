package net.jeedup.sql;

import net.jeedup.sql.impl.SqlConnection;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;

/**
 * Wrap a SQL transaction.
 * Created by zack on 9/12/18.
 */
public class Transaction extends SqlConnection implements AutoCloseable {

    private final Connection connection;
    private boolean wasAutoCommited = true;
    private long updates = 0;

    public Transaction(Connection connection) {
        super(connection);
        this.connection = connection;
    }

    public void begin() throws SQLException {
        wasAutoCommited = connection.getAutoCommit();
        connection.setAutoCommit(false);
    }

    public void end() throws SQLException {
        connection.commit();
        connection.setAutoCommit(wasAutoCommited);
        connection.close();
    }

    public void rollback() throws SQLException  {
        if (!connection.isClosed()) {
            connection.rollback();
            connection.close();
        }
    }

    @Override
    public int execute(String stmt) throws SQLException {
        int rows = 0;
        PreparedStatement preparedStatement = null;
        try {
            preparedStatement = connection.prepareStatement(stmt);
            rows = preparedStatement.executeUpdate();
            updates += rows;
        } catch (SQLException e) {
            rollback();
            throw e;
        } finally {
            if (preparedStatement != null) {
                preparedStatement.close();
            }
        }

        return rows;
    }

    public long getUpdates() {
        return updates;
    }

    @Override
    public void close() throws SQLException {
        end();
    }
}
