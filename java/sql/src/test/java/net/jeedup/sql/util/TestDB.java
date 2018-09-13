package net.jeedup.sql.util;

import com.opentable.db.postgres.embedded.EmbeddedPostgres;
import net.jeedup.sql.Sql;
import net.jeedup.sql.impl.SqlConnection;
import net.jeedup.sql.impl.SqlDataSource;
import org.postgresql.ds.PGSimpleDataSource;

import java.io.IOException;
import java.sql.DriverManager;

public class TestDB {

    /**
     * Create an in-memory Sql datasource using the H2 database.
     * @param databaseName name of the schema, must be unique for each use case
     * @return open sql datasource for a newly populated H2 database
     */
    public static Sql createTestingDatabase(String databaseName, String mode) {
        Sql sql = null;

        try {
            String url = String.format("jdbc:h2:mem:%s;mode=%s;DATABASE_TO_UPPER=FALSE", databaseName, mode);
            sql = new SqlConnection(DriverManager.getConnection(url));
        } catch (Exception e) {
            e.printStackTrace();
            throw new RuntimeException(e);
        }

        return sql;
    }

    /**
     * Create an embedded PostgreSQL Sql datasource.
     * @return newly created embedded postgresql db
     * @throws IOException on failure starting embedded postgres
     */
    public static TestPostgres createTestingPostgresDatabase() throws IOException {
        TestPostgres testpg = new TestPostgres();
        testpg.pg = EmbeddedPostgres.start();
        PGSimpleDataSource dataSource = new PGSimpleDataSource();
        dataSource.setUrl(testpg.pg.getJdbcUrl("postgres", "postgres"));
        dataSource.setUser("postgres");
        dataSource.setDatabaseName("postgres");
        testpg.sql = new SqlDataSource(dataSource);
        return testpg;
    }

    public static class TestPostgres {
        public EmbeddedPostgres pg;
        public Sql sql;
        public void close() throws Exception {
            if (sql != null)
                sql.close();
            sql = null;
            if (pg != null) {
                try {
                    pg.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            pg = null;
        }
    }
}
