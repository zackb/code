package net.jeedup.common.util;

import org.junit.jupiter.api.Test;

import java.util.UUID;

import static org.junit.jupiter.api.Assertions.*;

/**
 * Created by zack on 7/6/17.
 */
class UUIDsTest {

    @Test
    void testsCreate() {
        long ts = 1429473937613L;
        UUID uuid = UUIDs.timeBased(ts);
        assertEquals(ts, StringUtil.timestamp(uuid));
        assertEquals(ts, UUIDs.epoch(uuid));
        assertNotNull(UUIDs.random());
    }

}
