package net.jeedup.common.model;

/**
 * Marker interface to extract unix epoch millis timestamps
 * Created by zack on 2/11/19.
 */
public interface Timestamped {
    /**
     * Return the timestamp representing the time of a record
     * @return unix epoch milliseconds
     */
    long timestamp();
}
