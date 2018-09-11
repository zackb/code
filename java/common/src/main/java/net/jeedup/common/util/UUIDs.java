package net.jeedup.common.util;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.*;

/**
 * Utility methods to work with time-based version 1 UUIDs
 * Created by zack on 4/18/16.
 */
public final class UUIDs {

    private UUIDs() {}

    // http://www.ietf.org/rfc/rfc4122.txt
    private static final long START_EPOCH = makeEpoch();
    private static final long NODE = makeNode();

    public static long makeEpoch() {
        // UUID v1 timestamp must be in 100-nanoseconds interval since 00:00:00.000 15 Oct 1582.
        Calendar c = Calendar.getInstance(TimeZone.getTimeZone("GMT-0"));
        c.set(Calendar.YEAR, 1582);
        c.set(Calendar.MONTH, Calendar.OCTOBER);
        c.set(Calendar.DAY_OF_MONTH, 15);
        c.set(Calendar.HOUR_OF_DAY, 0);
        c.set(Calendar.MINUTE, 0);
        c.set(Calendar.SECOND, 0);
        c.set(Calendar.MILLISECOND, 0);
        return c.getTimeInMillis();
    }

    /**
     * Use system and software properties to generate a hash of this node
     * All v4 and v6 IP addresses
     * JRE information
     * OS information
     * @return hash to use as an identifier of this node
     */
    private static long makeNode() {

        try {

            MessageDigest digest = MessageDigest.getInstance("MD5");
            for (String address : addresses())
                update(digest, address);

            Properties props = System.getProperties();
            update(digest, props.getProperty("java.vendor"));
            update(digest, props.getProperty("java.vendor.url"));
            update(digest, props.getProperty("java.version"));
            update(digest, props.getProperty("os.arch"));
            update(digest, props.getProperty("os.name"));
            update(digest, props.getProperty("os.version"));

            byte[] hash = digest.digest();

            long node = 0;
            for (int i = 0; i < 6; i++)
                node |= (0x00000000000000ffL & (long)hash[i]) << (i*8);

            return node | 0x0000010000000000L;

        } catch (NoSuchAlgorithmException e) {
            throw new RuntimeException(e);
        }
    }

    private static void update(MessageDigest digest, String value) {
        if (value != null)
            digest.update(value.getBytes(StandardCharsets.UTF_8));
    }

    private static long makeClockSeqAndNode() {
        long clock = new Random(System.currentTimeMillis()).nextLong();
        long node = NODE;

        long lsb = 0;
        lsb |= (clock & 0x0000000000003FFFL) << 48;
        lsb |= 0x8000000000000000L;
        lsb |= node;
        return lsb;
    }

    /**
     * Creates a new random (version 4) UUID.
     * Convenience for {@code UUID.randomUUID()}.
     * @return a newly generated, pseudo random, version 4 UUID.
     */
    public static UUID random() {
        return UUID.randomUUID();
    }

    /**
     * Creates a new time-based (version 1) UUID.
     * @return a new time-based UUID.
     */
    public static UUID timeBased() {
        return timeBased(System.currentTimeMillis());
    }

    /**
     * Creates a new time-based (version 1) UUID using the provided unix epoch in millis
     * @param timestamp unix epoch in millis to use in the resultant UUID.timestamp
     * @return a new time-based UUID using timestamp.
     */
    public static UUID timeBased(long timestamp) {
        return new UUID(msb(fromUnixTimestamp(timestamp)), makeClockSeqAndNode());
    }

    /**
     * Return the unix timestamp contained by the provided time-based UUID.
     * @param uuid the UUID to return the timestamp of.
     * @return the unix timestamp of {@code uuid}.
     *
     * @throws IllegalArgumentException if {@code uuid} is not a version 1 UUID.
     */
    public static long epoch(UUID uuid) {
        if (uuid.version() != 1)
            throw new IllegalArgumentException("Invalid UUID version, no time-based information in UUIDs of version: " + uuid.version());

        long timestamp = uuid.timestamp();
        return (timestamp / 10000) + START_EPOCH;
    }

    private static long fromUnixTimestamp(long tstamp) {
        return (tstamp - START_EPOCH) * 10000;
    }

    /**
     * Create most sig bits to use to create a new UUID
     * @param timestamp to use
     * @return most significant bits for a new UUID v1 timestamp
     */
    private static long msb(long timestamp) {
        long msb = 0L;
        msb |= (0x00000000ffffffffL & timestamp) << 32;
        msb |= (0x0000ffff00000000L & timestamp) >>> 16;
        msb |= (0x0fff000000000000L & timestamp) >>> 48;
        msb |= 0x0000000000001000L; // sets the version to 1.
        return msb;
    }

    /**
     * Get a string rep of all local network interfaces
     * @return ip dotted string for each local network interface
     */
    private static Set<String> addresses() {
        Set<String> ips = new HashSet<>();
        try {
            Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces();
            if (en != null) {
                while (en.hasMoreElements()) {
                    Enumeration<InetAddress> addrs = en.nextElement().getInetAddresses();
                    while (addrs.hasMoreElements())
                        ips.add(addrs.nextElement().toString()); // ip
                }
            }

        } catch (SocketException e1) { /** NOOP **/ }

        return ips;
    }
}
