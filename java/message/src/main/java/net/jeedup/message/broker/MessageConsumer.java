package net.jeedup.message.broker;

import net.jeedup.message.Message;

/**
 * Consume messges from a broker
 * Created by zack on 9/11/18.
 */
public interface MessageConsumer<T> {
    /**
     * TODO
     * @param message
     */
    void consume(Message<T> message);
}
