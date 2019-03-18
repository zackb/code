package net.jeedup.common.time;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

class ExpiryTest {

    @Test
    void testBasic() {
        // 1 hour expiration
        long exp = 1000L * 60 * 60;

        // 01/09/2018 06:00:00 UTC
        long created = 1515477600000L;

        Expiry expiry = new Expiry(exp, null);

        long life = expiry.createExpiry(created);

        assertFalse(expiry.expired(life, created));
        assertFalse(expiry.expired(life, created + 13409));
        assertFalse(expiry.expired(life, created + exp - 2));
        assertTrue(expiry.expired(life, created + exp));
        assertTrue(expiry.expired(life, created + created));
    }

    @Test
    void testWindow() {
        // 4 hour expiration
        long exp = 1000L * 60 * 60 * 4;

        // 15 minute window
        long window = 1000 * 60 * 15;

        // 01/09/2018 06:00:00 UTC
        long created = 1515477600000L;

        Expiry expiry = new Expiry(exp, window);

        long life = expiry.createExpiry(created);

        assertFalse(expiry.expired(life, created));
        assertFalse(expiry.expired(life, created + 13409));
        assertFalse(expiry.expired(life, created + exp - window - 1));

        assertFalse(expiry.expired(life, created + exp - window));
        assertTrue(expiry.expired(life, created + exp));

        // 01/09/2018 06:14:00 UTC
        created = 1515478440000L;

        life = expiry.createExpiry(created);

        assertFalse(expiry.expired(life, created));
        assertFalse(expiry.expired(life, created + 13409));

        // 01/09/2018 08:14:00 GMT
        assertFalse(expiry.expired(life, 1515485640000L));

        // 01/09/2018 09:59:00 GMT
        assertFalse(expiry.expired(life, 1515491940000L));

        // 01/09/2018 10:00:00 GMT
        assertTrue(expiry.expired(life, 1515492000000L));

        // 01/09/2018 10:14:00 GMT
        assertTrue(expiry.expired(life, 1515492840000L));

    }
}