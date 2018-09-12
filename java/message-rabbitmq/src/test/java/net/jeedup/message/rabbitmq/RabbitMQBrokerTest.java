package net.jeedup.message.rabbitmq;

import net.jeedup.message.broker.MessageBroker;
import net.jeedup.message.broker.impl.BaseMessage;
import net.jeedup.message.impl.JavaSerde;
import org.junit.jupiter.api.Test;

import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.CountDownLatch;
import java.util.logging.Logger;

import static net.jeedup.common.collection.CollectionUtil.set;
import static net.jeedup.common.util.Util.empty;
import static org.junit.jupiter.api.Assertions.*;

class RabbitMQBrokerTest {

    private static final Logger log = Logger.getLogger(RabbitMQBrokerTest.class.getName());

    @Test
    void testSimple() throws Exception {
        RabbitMQConfig config = new RabbitMQConfig();
        //config.host = "127.0.0.1";
        config.autoDelete = true;
        config.durable = false;
        config.queueName = "unit-test";
        config.serde = new JavaSerde<>();

        if (empty(config.host)) {
            log.info("Skipping RabbitMQ test because no host specified");
            return;
        }

        RabbitMQBroker<String> broker = new RabbitMQBroker<>(config);
        broker.startup();

        CountDownLatch latch = new CountDownLatch(3);

        BaseMessage<String> m = new BaseMessage<>();
        m.setPayload("test1");
        broker.publish(m);

        m = new BaseMessage<>();
        m.setPayload("test2");
        broker.publish(m);

        Set<String> rcv = new HashSet<>(3);

        broker.consume(msg -> {
            rcv.add(msg.getPayload());
            latch.countDown();
            msg.ack();
        });

        m = new BaseMessage<>();
        m.setPayload("test3");
        broker.publish(m);

        latch.await();
        broker.shutdown();

        assertEquals(3, rcv.size());

        assertEquals(set("test1", "test2", "test3"), rcv);
    }

}