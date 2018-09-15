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
        //postgres = TestDB.createTestingPostgresDatabase();
        mysql = TestDB.createTestingDatabase("SqlDBTest", "MySql");
        // mysql.execute("create table test_object (id varchar(255) primary key, itgr int(10), data varchar(255), dbl numeric, updated datetime, created datetime)");
    }

    @AfterAll
    static void destroy() throws Exception {
        //postgres.close();
        mysql.close();
    }

    @Test
    void testSave() throws Exception {
        SqlDB<TestObject> db = new SqlDB<>(TestObject.class, mysql);
        db.createTable();
        TestObject o1 = new TestObject();
        o1.id = "foo";
        o1.itgr = 43;
        o1.dbl = 42.2;
        db.save(o1);

        TestObject o2 = db.get("foo");
        assertNotNull(o2);
        assertEquals("foo", o2.id);
        assertEquals(43, o2.itgr);
        assertEquals(42.2, o2.dbl);
        assertNull(o2.data);

        assertNotNull(o2.updated);
        assertNotNull(o2.created);

        o2.itgr = 4;
        o2.data = "Hello";
        db.save(o2);

        TestObject o3 = db.get("foo");
        assertNotNull(o3);
        assertEquals("foo", o3.id);
        assertEquals(4, o3.itgr);
        assertEquals(42.2, o3.dbl);
        assertEquals("Hello", o3.data);
        assertNotNull(o3.updated);
        assertNotNull(o3.created);

        TestObject o4 = new TestObject();
        o4.id = "foo";
        o4.dbl = 22.9;
        db.update(o4);

        TestObject o5 = db.get("foo");
        assertNotNull(o5);
        assertEquals("foo", o5.id);
        assertEquals(0, o5.itgr);
        assertEquals(22.9, o5.dbl);
        assertNull(o5.data);
        assertNotNull(o5.updated);
        assertNotNull(o5.created);
    }

    static class TestObject {
        String id;
        int itgr;
        double dbl;
        // Double ddbl;
        // long lng;
        // Long llng;
        String data;
        // byte[] bytes;
        Date updated;
        Date created;
    }

}