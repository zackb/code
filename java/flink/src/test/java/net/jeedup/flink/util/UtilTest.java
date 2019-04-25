package net.jeedup.flink.util;

import org.junit.jupiter.api.Test;

import static net.jeedup.flink.util.Util.*;
import static org.junit.jupiter.api.Assertions.*;

class UtilTest {

    @Test
    void testCompare() {
        String low = "s3a://bucket_name/json/2019/02/23";
        String high = "s3a://bucket_name/json/2019/03/01";

        assertTrue(greaterThanEqual(low, low));
        assertFalse(lessThan(low, low));
        assertTrue(greaterThanEqual(high, high));
        assertFalse(lessThan(high, high));

        String s2 = "s3a://bucket_name/json/2019/02/23/10/Foo.gz";
        assertTrue(greaterThanEqual(low, s2));
        assertFalse(greaterThanEqual(s2, low));
        assertFalse(lessThan(low, s2));
        assertTrue(lessThan(s2, low));


        String test = "s3a://bucket_name/json/2019/02/23/00/DataFirehose-2-2019-02-23-00-00-08.gz";

        assertTrue(greaterThanEqual(low, test));
        assertTrue(lessThan(high, test));

        String lowerBound = low;
        String higherBound = high;

        assertTrue(checkBounds(lowerBound, higherBound, test));
        lowerBound = null;
        assertTrue(checkBounds(lowerBound, higherBound, test));
        lowerBound = low;
        higherBound = null;
        assertTrue(checkBounds(lowerBound, higherBound, test));
        lowerBound = null;
        higherBound = null;
        assertTrue(checkBounds(lowerBound, higherBound, test));

        test = "s3a://bucket_name/json/2019/03/01/00/DataFirehose-2-2019-02-23-00-00-08.gz";

        lowerBound = low;
        higherBound = high;

        assertFalse(checkBounds(lowerBound, higherBound, test));
        lowerBound = null;
        assertFalse(checkBounds(lowerBound, higherBound, test));
        lowerBound = low;
        higherBound = null;
        assertTrue(checkBounds(lowerBound, higherBound, test));
        lowerBound = null;
        higherBound = null;
        assertTrue(checkBounds(lowerBound, higherBound, test));
    }


}
