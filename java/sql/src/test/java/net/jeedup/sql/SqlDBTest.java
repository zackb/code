package net.jeedup.sql;

import net.jeedup.sql.util.TestDB;
import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;

import java.util.Date;

import static org.junit.jupiter.api.Assertions.*;

class SqlDBTest {

    private static TestDB.TestPostgres postgres;
    private static Sql mysql;

    @BeforeAll
    static void init() throws Exception {
        postgres = TestDB.createTestingPostgresDatabase();
        mysql = TestDB.createTestingDatabase("SqlDBTest", "MySql");
        mysql.execute("create table test_object (id varchar(255), itgr int(10))");
    }

    @AfterAll
    static void destroy() throws Exception {
        postgres.close();
        mysql.close();
    }

    @Test
    void testInsert() throws Exception {
        SqlDB<TestObject> db = new SqlDB<>(TestObject.class, mysql);
        TestObject o1 = new TestObject();
        o1.id = "foo";
        o1.itgr = 43;
        db.save(o1);
        mysql.eachRow("select * from test_object", row -> {
            System.out.println(row);
        });
        TestObject o2 = db.get("foo");
        assertNotNull(o2);
        assertEquals("foo", o2.id);
        assertEquals(43, o2.itgr);
    }

    static class TestObject {
        String id;
        int itgr;
        // double dbl;
        // Double ddbl;
        // long lng;
        // Long llng;
        // String data;
        // byte[] bytes;
        // Date updated;
        // Date created;
    }

}