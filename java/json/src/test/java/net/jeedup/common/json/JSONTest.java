package net.jeedup.common.json;

import net.jeedup.common.net.HostAndPort;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

class JSONTest {

    @Test
    void testParseHostAndPort() {
        String json = "{ \"host\": \"google.com\", \"port\": 1111 }";
        HostAndPort hostAndPort = JSON.decodeObject(json, HostAndPort.class);
        assertNotNull(hostAndPort);
        assertEquals("google.com", hostAndPort.host);
        assertEquals(1111, hostAndPort.port.intValue());
    }
}