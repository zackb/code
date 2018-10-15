package net.jeedup.common.util;

import org.junit.jupiter.api.Test;

import java.util.Arrays;
import java.util.Map;

import static net.jeedup.common.util.StringUtil.isCapitalized;
import static net.jeedup.common.util.StringUtil.snake;
import static org.junit.jupiter.api.Assertions.*;

/**
 * Created by zack on 6/6/17.
 */
class StringUtilTest {

    @Test
    void testLpad() {
        String s = "hello";
        assertEquals("ZZZZZhello", StringUtil.lpad(s, 10, 'Z'));
    }

    @Test
    void testRpad() {
        String s = "hello";
        assertEquals("helloZZZZZ", StringUtil.rpad(s, 10, 'Z'));
    }

    @Test
    void testSplitQuery() {
        String query = "?foo=bar&baz=zoo&1=2";
        Map<String, String> parts = StringUtil.splitQuery(query);
        assertEquals(3, parts.size());
        assertTrue(parts.keySet().containsAll(Arrays.asList("foo", "baz", "1")));
        assertTrue(parts.values().containsAll(Arrays.asList("bar", "zoo", "2")));
        assertEquals("bar", parts.get("foo"));
        assertEquals("zoo", parts.get("baz"));
        assertEquals("2", parts.get("1"));
    }

    @Test
    void testSplitQueryBogusData() {
        String query = "?v=418bf589b01562deb5419ee09a6c876f&k=QWFLcVR5cHdQaFZqbXFzZWk3RCtTaW9Vcmo0PV97ImEiOiI5MSwsamFKUCxlblVTLjEiLCJjIjoxNDc0OTEyMzIzLCJkIjoiY3JhbmltZSIsImciOiJaWiIsImgiOiI3b3F4aWZna3E0aTJ1Z2oiLCJsIjo3MjAwLCJwIjoiMSIsInIiOiJlMjNjNjgiLCJzIjo0NjgxMzYsInQiOjE0NzkzOTM4NzUsInYiOjN9&Policy=ewoiU3RhdGVtZW50IjpbCnsKIlJlc291cmNlIjoiaHR0cHM6Ly9kbC52LnZydi5jby9ldnMvYXNzZXRzLzdvcXhpZmdrcTRpMnVnal8sMTU5NDA2Ny5tcDQsMTU5NDA2OS5tcDQsMTU5NDA3MS5tcDQsMTU5NDA3My5tcDQsMTU5NDA3NS5tcDQsLnVybHNldC9tYXN0ZXIubTN1OD92PTQxOGJmNTg5YjAxNTYyZGViNTQxOWVlMDlhNmM4NzZmJms9UVdGTGNWUjVjSGRRYUZacWJYRnpaV2szUkN0VGFXOVZjbW8wUFY5N0ltRWlPaUk1TVN3c2FtRktVQ3hsYmxWVExqRWlMQ0pqSWpveE5EYzBPVEV5TXpJekxDSmtJam9pWTNKaGJtbHRaU0lzSW1jaU9pSmFXaUlzSW1naU9pSTNiM0Y0YVdabmEzRTBhVEoxWjJvaUxDSnNJam8zTWpBd0xDSndJam9pTVNJc0luSWlPaUpsTWpOak5qZ2lMQ0p6SWpvME5qZ3hNellzSW5RaU9qRTBOemt6T1RNNE56VXNJbllpT2pOOSIsCiJDb25kaXRpb24iOnsKIkRhdGVMZXNzVGhhbiI6eyJBV1M6RXBvY2hUaW1lIjoxNDc5NDA4Mjc1fQp9Cn0KXQp9&Signa";
        Map<String, String> parts = StringUtil.splitQuery(query);
        assertNotNull(parts);
        assertEquals(3, parts.size());
        assertEquals("418bf589b01562deb5419ee09a6c876f", parts.get("v"));
        assertEquals("QWFLcVR5cHdQaFZqbXFzZWk3RCtTaW9Vcmo0PV97ImEiOiI5MSwsamFKUCxlblVTLjEiLCJjIjoxNDc0OTEyMzIzLCJkIjoiY3JhbmltZSIsImciOiJaWiIsImgiOiI3b3F4aWZna3E0aTJ1Z2oiLCJsIjo3MjAwLCJwIjoiMSIsInIiOiJlMjNjNjgiLCJzIjo0NjgxMzYsInQiOjE0NzkzOTM4NzUsInYiOjN9", parts.get("k"));
        assertNotNull(parts.get("Policy"));
    }

    @Test
    void testTruncate() {
        String str = "12345678";
        assertEquals("12345678", StringUtil.truncate(str, 10));
        assertEquals("12345678", StringUtil.truncate(str, 8));
        assertEquals("1234567", StringUtil.truncate(str, 7));
        assertEquals("1", StringUtil.truncate(str,1));
        assertEquals("", StringUtil.truncate(str,0));
    }

    @Test
    void testSnakeCase() {
        assertEquals("this_is_test", snake("thisIsTest"));
    }

    @Test
    void testIsCapitalized() {
        assertTrue(isCapitalized("Zack"));
        assertFalse(isCapitalized("ZackBartel"));
        assertFalse(isCapitalized("Zack Bartel"));
        assertFalse(isCapitalized("zAck"));
    }
}
