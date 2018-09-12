package net.jeedup.message.broker.impl;

import net.jeedup.message.Message;
import net.jeedup.message.broker.MessageBroker;

/**
 * POJO Message implementation
 *
 * Created by zack on 9/11/18.
 */
public class BaseMessage<T> implements Message<T> {

    private T payload;
    private long timestamp = System.currentTimeMillis();

    public MessageBroker.Ack ack = () -> System.err.println("ACKING NOT ENABLED");

    @Override
    public T getPayload() {
        return payload;
    }

    @Override
    public void setPayload(T payload) {
        this.payload = payload;
    }

    @Override
    public long getTimestamp() {
        return timestamp;
    }

    @Override
    public void setTimestamp(long timestamp) {
        this.timestamp = timestamp;
    }

    @Override
    public void ack() {
        ack.ack();
    }
}
