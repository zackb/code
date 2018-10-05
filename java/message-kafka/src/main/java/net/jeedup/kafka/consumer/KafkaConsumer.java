package net.jeedup.kafka.consumer;

import net.jeedup.kafka.KafkaConfig;
import net.jeedup.message.broker.MessageConsumer;
import net.jeedup.message.broker.impl.BaseMessage;
import org.apache.kafka.clients.consumer.Consumer;
import org.apache.kafka.clients.consumer.ConsumerConfig;
import org.apache.kafka.clients.consumer.ConsumerRecords;
import org.apache.kafka.common.serialization.StringDeserializer;

import java.time.Duration;
import java.util.Properties;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

import static java.util.Collections.singletonList;
import static net.jeedup.message.broker.BrokerUtil.createClientId;

/**
 * A simple Kafka consumer
 *
 * Created by zack on 10/2/18.
 */
public class KafkaConsumer<T> {

    private final String topic;

    private final Consumer<String, T> consumer;

    private final AtomicBoolean shutdown;

    private final CountDownLatch shutdownLatch;

    private final ExecutorService executor = Executors.newSingleThreadExecutor();


    @SuppressWarnings("unchecked")
    public KafkaConsumer(final KafkaConfig config) {

        topic = config.topic;

        shutdown = new AtomicBoolean(false);

        shutdownLatch = new CountDownLatch(1);

        Properties props = new Properties();

        props.put(ConsumerConfig.CLIENT_ID_CONFIG, createClientId());

        props.put(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG, config.bootstrapServerString());

        props.put(ConsumerConfig.GROUP_ID_CONFIG, config.groupName);

        props.put(ConsumerConfig.KEY_DESERIALIZER_CLASS_CONFIG, StringDeserializer.class.getName());

        props.put(ConsumerConfig.VALUE_DESERIALIZER_CLASS_CONFIG, config.serdeClassName);

        consumer = new org.apache.kafka.clients.consumer.KafkaConsumer<>(props);
    }

    public void shutdown() throws Exception {
        shutdown.set(true);
        shutdownLatch.await(4, TimeUnit.SECONDS);
        executor.shutdown();
    }

    public void consume(MessageConsumer<T> consume) {
        executor.submit(() -> {
            try {
                consumer.subscribe(singletonList(topic));

                while (!shutdown.get()) {
                    ConsumerRecords<String, T> messages = consumer.poll(Duration.ofSeconds(2));

                    messages.forEach(msg -> {
                        BaseMessage<T> message = new BaseMessage<>();
                        message.setPayload(msg.value());
                        message.ack = () -> {};
                        consume.consume(message);
                    });
                }
            } finally {
                consumer.close();
                shutdownLatch.countDown();
            }
        });
    }
}
