package net.jeedup.message;

/**
 * Wrapper to send and receive data to/from a message broker
 * Created by zack on 9/11/18.
 */
public interface Message<T> {

    T getPayload();

    void setPayload(T payload);

    long getTimestamp();

    void setTimestamp(long timestamp);

    void ack();

}
