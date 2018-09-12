package net.jeedup.message.broker.impl;

import net.jeedup.message.broker.MessageBroker;
import org.junit.jupiter.api.Test;

import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.CountDownLatch;

import static net.jeedup.common.collection.CollectionUtil.set;
import static org.junit.jupiter.api.Assertions.*;

class InMemoryBrokerTest {

    @Test
    void testSimple() throws Exception {
        MessageBroker<String> broker = new InMemoryBroker<>();
        broker.startup();

        CountDownLatch latch = new CountDownLatch(3);

        MessageBroker.Ack ack = () -> latch.countDown();

        BaseMessage<String> m = new BaseMessage<>();
        m.setPayload("test1");
        m.ack = ack;
        broker.publish(m);

        m = new BaseMessage<>();
        m.setPayload("test2");
        m.ack = ack;
        broker.publish(m);

        Set<String> rcv = new HashSet<>(3);

        broker.consume(msg -> {
            rcv.add(msg.getPayload());
            msg.ack();
        });

        m = new BaseMessage<>();
        m.setPayload("test3");
        m.ack = ack;
        broker.publish(m);

        latch.await();
        broker.shutdown();

        assertEquals(3, rcv.size());

        assertEquals(set("test1", "test2", "test3"), rcv);
    }

}