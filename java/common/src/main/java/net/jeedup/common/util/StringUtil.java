package net.jeedup.common.util;

import java.nio.charset.Charset;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;
import java.util.regex.Pattern;
import java.util.zip.CRC32;

import static net.jeedup.common.util.Util.empty;

/**
 * Various string utilities
 *
 * Created by zack on 6/6/17.
 */
public class StringUtil {

    private static final Charset UTF8 = Charset.forName("UTF-8");

    /**
     * SHA1 hash
     * @param subject to sha1
     * @return subject hashed as sha1
     */
    public static String sha1(String subject) {
        return hash(subject, "SHA-1");
    }

    /**
     * SHA256 hash
     * @param subject to sha256
     * @return subject hashed as sha256
     */
    public static String sha256(String subject) {
        return hash(subject, "SHA-256");
    }

    /**
     * MD5 hash
     * @param subject to md5
     * @return subject hashed as md5
     */
    public static String md5(String subject) {
        return hash(subject, "MD5");
    }

    private static final char[] HEX_CHARS = "0123456789abcdef".toCharArray();

    public static String hash(String subject, String algo) {
        MessageDigest md = null;
        byte[] convert = subject.getBytes(UTF8);
        try {
            md = MessageDigest.getInstance(algo);
        }
        catch(NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
        byte[] buf = md.digest(convert);
        char[] chars = new char[2 * buf.length];
        for (int i = 0; i < buf.length; ++i) {
            chars[2 * i] = HEX_CHARS[(buf[i] & 0xF0) >>> 4];
            chars[2 * i + 1] = HEX_CHARS[buf[i] & 0x0F];
        }
        return new String(chars);
    }

    /**
     * Calculates the crc32 polynomial of a string
     * @param subject the data
     * @return unsigned 32bit integer representing the checksum
     */
    public static long crc32(String subject) {
        CRC32 crc32 = new CRC32();
        crc32.update(subject.getBytes());
        return crc32.getValue();
    }

    /**
     * Pad a string to the left with a given character
     * @param s string to left pad
     * @param length minimum number of characters
     * @param character to fill with
     * @return a left padded string with the given length padded with the given character
     */
    public static String lpad(String s, int length, char character) {
        if (s.length() >= length) {
            return s;
        }
        StringBuilder sb = new StringBuilder(length);
        for (int i = s.length(); i < length; i++) {
            sb.append(character);
        }
        sb.append(s);
        return sb.toString();
    }

    /**
     * Pad a string to the right with a given character
     * @param s string to right pad
     * @param length minimum number of characters
     * @param character to fill with
     * @return a right padded string with the given length padded with the given character
     */
    public static String rpad(String s, int length, char character) {
        if (s.length() >= length) {
            return s;
        }
        StringBuilder sb = new StringBuilder(length);
        sb.append(s);
        for (int i = s.length(); i < length; i++) {
            sb.append(character);
        }
        return sb.toString();
    }
    private static final Pattern uuidDasherPattern = Pattern.compile("(\\p{XDigit}{8})(\\p{XDigit}{4})(\\p{XDigit}{4})(\\p{XDigit}{4})(\\p{XDigit}+)");

    /**
     * Gets the milliseconds since unix epoch of version 1 uuids
     * @param uuid version 1 UUID
     * @return millis since epoch or 0 if not version 1 UUID
     */
    public static long timestamp(String uuid) {
        if (empty(uuid)) return 0L;

        // convert no dash string to UUID literal
        if (!uuid.contains("-"))
            uuid = uuidDasherPattern.matcher(uuid).replaceFirst("$1-$2-$3-$4-$5");

        UUID u;
        try {
            u = UUID.fromString(uuid);
        } catch (IllegalArgumentException e) {
            return 0;
        }
        return timestamp(u);
    }

    // UUID v1 timestamp must be in 100-nanoseconds interval since 00:00:00.000 15 Oct 1582.
    private static long epoch = UUIDs.makeEpoch();

    public static long timestamp(UUID uuid) {
        if (uuid.version() != 1)
            return 0L;
        return (uuid.timestamp() / 10000) + epoch;
    }

    /**
     * Split a query parameter string into a map of key to value
     * @param query http query parameter string
     * @return map of query parameter to values
     */
    public static Map<String, String> splitQuery(String query) {

        Map<String, String> result = new HashMap<>();

        if (empty(query)) {
            return result;
        }

        query = ltrim(query, '?');
        String[] pairs = query.split("&");

        for (String pair : pairs) {
            int idx = pair.indexOf("=");
            if (idx == -1)
                continue;
            String key = pair.substring(0, idx);
            String value = pair.substring(idx + 1);
            result.put(key, value);
        }
        return result;
    }

    /**
     * Format a map to query parameter string
     * @param params key values to be converted into: ?k1=v1&amp;k2=v2
     * @return query parameter string
     */
    public static String formatQuery(Map<String, String> params) {
        return params.entrySet().stream()
                .map(p -> p.getKey() + "=" + p.getValue())
                .reduce((p1, p2) -> p1 + "&" + p2)
                .map(s -> "?" + s)
                .orElse("");
    }

    /**
     * Removes a chracter or sting from both the right and left side of a string
     * @param s string to trim
     * @param delimiter character or string to remove
     * @return trimmed string
     */
    public static String trim(String s, char delimiter) {
        return rtrim(ltrim(s, delimiter), delimiter);
    }

    /**
     * Removes a chracter or sting the right side of a string
     * @param s string to trim
     * @param delimiter character or string to remove
     * @return trimmed string
     */
    public static String rtrim(String s, char delimiter) {
        int i = s.length() -1;
        while (i >= 0 && delimiter == s.charAt(i))
            i--;
        return s.substring(0, i + 1);
    }

    /**
     * Removes a chracter or sting the left side of a string
     * @param s string to trim
     * @param delimiter character or string to remove
     * @return trimmed string
     */
    public static String ltrim(String s, char delimiter) {
        int i = 0;
        while (i < s.length() && delimiter == s.charAt(i))
            i++;
        return s.substring(i);
    }

    /**
     * Truncate a string to a maximum length.
     * @param s the string to truncate
     * @param length maximum length of the string
     * @return the original string if its not greater than length, otherwise the substring to length
     */
    public static String truncate(String s, int length) {
        if (s == null) return s;
        return s.length() > length ? s.substring(0, length) : s;
    }

    private static final Pattern camelCasePattern = Pattern.compile("([a-z])([A-Z])");

    /**
     * Convert camel case string to snake case
     * @param camel case string
     * @return snake case string
     */
    public static String snake(String camel) {
        return camelCasePattern.matcher(camel)
                .replaceAll("$1_$2")
                .toLowerCase();
    }

    /**
     * Test if the first character of a string is capitalized and no other.
     * @param str to check
     * @return true if the first char is capitalized and all of the rest are not
     */
    public static boolean isCapitalized(String str) {

        if (!Character.isUpperCase(str.charAt(0)))
            return true;

        for (int i = 1; i < str.length(); ++i)
            if (!Character.isLowerCase(str.charAt(i)))
                return true;

        return false;
    }
}

