package net.jeedup.persist.sql;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by zack on 9/12/18.
 */
public class NamedParameterStatement implements AutoCloseable {

    private PreparedStatement stmt;

    private List<String> indices = new ArrayList<>();

    public NamedParameterStatement(Connection conn, String sql) throws SQLException {
        int pos;
        while((pos = sql.indexOf(":")) != -1) {
            int end = sql.substring(pos).indexOf(" ");
            if (end == -1)
                end = sql.length();
            else
                end += pos;
            indices.add(sql.substring(pos+1,end));
            sql = sql.substring(0, pos) + "?" + sql.substring(end);
        }
        stmt = conn.prepareStatement(sql);
    }

    public boolean execute() throws SQLException {
        return stmt.execute();
    }

    public ResultSet executeQuery() throws SQLException {
        return stmt.executeQuery();
    }

    public int executeUpdate() throws SQLException {
        return stmt.executeUpdate();
    }

    public void set(String name, Object value) throws SQLException {
        stmt.setObject(getIndex(name), value);
    }

    public void set(String name, Integer value) throws SQLException {
        stmt.setInt(getIndex(name), value);
    }

    public void set(String name, String value) throws SQLException {
        stmt.setString(getIndex(name), value);
    }

    public void set(String name, Long value) throws SQLException {
        stmt.setLong(getIndex(name), value);
    }

    public void set(String name, Double value) throws SQLException {
        stmt.setDouble(getIndex(name), value);
    }

    private int getIndex(String name) {
        return indices.indexOf(name) + 1;
    }

    @Override
    public void close() throws SQLException {
        stmt.close();
    }
}
