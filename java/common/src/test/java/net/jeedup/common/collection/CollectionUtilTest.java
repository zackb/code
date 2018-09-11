package net.jeedup.common.collection;

import org.junit.jupiter.api.Test;

import java.util.HashSet;
import java.util.Set;

import static org.junit.jupiter.api.Assertions.*;

class CollectionUtilTest {

    @Test
    void testToSet() {
        String[] elements = new String[] { "one", "two", "three", "one"};
        Set<String> expected = new HashSet<>();
        expected.add("one");
        expected.add("two");
        expected.add("three");
        Set<String> set = CollectionUtil.set(elements);
        assertNotNull(set);
        assertEquals(3, set.size());
        assertEquals(expected, set);
    }

}
