package net.jeedup.message.broker;

import net.jeedup.message.Message;

/**
 * Interface to a message broker
 * Created by zack on 9/11/18.
 */
public interface MessageBroker<T> {

    void publish(Message<T> message) throws Exception;

    void consume(MessageConsumer<T> consumer) throws Exception;

    void startup() throws Exception;

    void shutdown() throws Exception;

    @FunctionalInterface
    interface Ack {
        void ack();
    }
}
