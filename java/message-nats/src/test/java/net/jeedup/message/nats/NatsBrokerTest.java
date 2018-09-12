package net.jeedup.message.nats;

import net.jeedup.common.net.HostAndPort;
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

class NatsBrokerTest {

    private static final Logger log = Logger.getLogger(NatsBrokerTest.class.getName());

    @Test
    void testBasic() throws Exception {
        NatsConfig config = new NatsConfig();

        config.hosts = set(new HostAndPort("127.0.0.1", 4222));

        config.hosts = null;

        config.subject = "unit-test";
        config.serde = new JavaSerde<>();

        if (empty(config.hosts)) {
            log.info("Skipping RabbitMQ test because no host specified");
            return;
        }

        MessageBroker<String> broker = new NatsBroker<>(config);
        broker.startup();

        Set<String> rcv = new HashSet<>(3);

        CountDownLatch latch = new CountDownLatch(3);

        broker.consume(msg -> {
            rcv.add(msg.getPayload());
            latch.countDown();
            msg.ack();
        });

        BaseMessage<String> m = new BaseMessage<>();
        m.setPayload("test1");
        broker.publish(m);

        m = new BaseMessage<>();
        m.setPayload("test2");
        broker.publish(m);

        m = new BaseMessage<>();
        m.setPayload("test3");
        broker.publish(m);

        latch.await();
        broker.shutdown();

        assertEquals(3, rcv.size());

        assertEquals(set("test1", "test2", "test3"), rcv);
    }

}