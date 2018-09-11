package net.jeedup.common.util;

import org.junit.jupiter.api.Test;

import static net.jeedup.common.util.Util.empty;
import static org.junit.jupiter.api.Assertions.*;

/**
 * Created by zack on 3/1/18.
 */
class UtilTest {
    @Test
    void testEmptyNumber() {
        Long zero = 0L;
        assertTrue(empty(zero));
    }

    @Test
    void testEmptyDouble() {
        Double zero = 0.0d;
        assertTrue(empty(zero));
        zero = Double.NaN;
        assertTrue(empty(zero));
        zero = 0.0000001;
        assertFalse(empty(zero));
    }

    @Test
    void testEmptyShort() {
        Short zero = 0;
        assertTrue(empty(zero));
        zero = 10;
        assertFalse(empty(zero));
    }

    @Test
    void testEmptyInteger() {
        int foo = 0;
        assertTrue(empty(foo));
        Integer fee = null;
        assertTrue(empty(fee));
        Integer baz = 0;
        assertTrue(empty(baz));
    }
}
