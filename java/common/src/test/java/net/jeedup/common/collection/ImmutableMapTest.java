package net.jeedup.common.collection;

import org.junit.jupiter.api.Test;

import java.util.Map;

import static org.junit.jupiter.api.Assertions.*;

class ImmutableMapTest {

    @Test
    void testBasic() {
        Map<String, Integer> map = ImmutableMap.<String, Integer>builder()
                .put("foo", 1)
                .build();

        assertEquals(1, map.size());
        assertEquals(1, map.get("foo").intValue());

        assertThrows(UnsupportedOperationException.class, () -> map.put("foo", 2));

        assertEquals(1, map.size());
        assertEquals(1, map.get("foo").intValue());
    }

}
