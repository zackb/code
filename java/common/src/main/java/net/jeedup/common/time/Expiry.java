package net.jeedup.common.time;

import java.io.Serializable;

/**
 * Utility to deal with expiry and expired testing.
 * Sometimes I really need to dumb it down for myself...
 *
 * Created by zack on 3/18/19.
 */
public class Expiry implements Serializable {

    // lifetime in milliseconds
    private final long expiry;

    // to reduce the number of unique expiration times, round expiry to nearest X millis
    private final long window;

    /**
     * Create a new expiry tracking utility.
     * @param expiry the number of milliseconds an object should live
     * @param window optional window of time to use to round the generated expiry and checks.
     *               This is useful to reduce the number of Flink timers keeping track.
     *               Ex: expiry = 1 hour, window = 10 minutes. The object could expire between 50 and 60 minutes
     */
    public Expiry(long expiry, Long window) {
        this.expiry = expiry;
        this.window = window == null ? 1 : window;
    }

    /**
     * Create an expiry given a timestamp. Add the lifetime {@link #expiry} to the timestamp and round down to the
     * nearest {@link #window} if necessary.
     * @param timestamp  the timestamp to create an expiry for. Add {@link #expiry} and round to {@link #window}
     * @return a timestamp rounded down to the nearest {@link #window}
     */
    public long createExpiry(long timestamp) {
        return ((timestamp + expiry) / window) * window;
    }

    /**
     * Check if a timestamp is expired
     * @param timestamp the timestamp (updated) to see if is expired
     * @param now the current (event) time to check expiration against
     * @return true if this timestamp is expired
     */
    public boolean expired(long timestamp, long now) {
        return now >= timestamp;
    }

}
