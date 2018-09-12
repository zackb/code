package net.jeedup.message.impl;

import net.jeedup.message.Serde;
import org.junit.jupiter.api.Test;

import java.io.Serializable;
import java.util.Date;

import static org.junit.jupiter.api.Assertions.*;

class JavaSerdeTest {

    static class TestObj implements Serializable {
        String str;
        int integer;
        Date date;
    }

    @Test
    void testSerDe() {

        Serde<TestObj> serde = new JavaSerde<>();

        Date date = new Date();
        TestObj obj = new TestObj();
        obj.str = "myString";
        obj.integer = 54;
        obj.date = date;

        byte[] bytes = serde.serialize(obj);
        TestObj result = serde.deserialize(bytes);

        assertNotNull(result);
        assertEquals("myString", obj.str);
        assertEquals(54, obj.integer);
        assertEquals(date, obj.date);
    }
}