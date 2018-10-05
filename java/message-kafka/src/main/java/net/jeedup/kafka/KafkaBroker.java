package net.jeedup.kafka;

import net.jeedup.kafka.consumer.KafkaConsumer;
import net.jeedup.kafka.producer.KafkaProducer;
import net.jeedup.message.Message;
import net.jeedup.message.broker.MessageBroker;
import net.jeedup.message.broker.MessageConsumer;

import java.util.Collection;

import static java.util.stream.Collectors.toList;

/**
 * Kafka broker just for kicks, probably would want to use just the
 * {@link net.jeedup.kafka.consumer.KafkaConsumer} or {@link net.jeedup.kafka.producer.KafkaProducer}
 * alone for the right use case.
 * Created by zack on 10/3/18.
 */
public class KafkaBroker<T> implements MessageBroker<T> {

    private final KafkaProducer<T> producer;
    private final KafkaConsumer<T> consumer;

    public KafkaBroker(KafkaConfig config) {
        this.producer = new KafkaProducer<>(config);
        this.consumer = new KafkaConsumer<>(config);
    }

    @Override
    public void publish(Message<T> message) throws Exception {
        producer.publish(message.getPayload());
    }

    @Override
    public void publish(Collection<Message<T>> messages) throws Exception {
        producer.publish(messages.stream()
                .map(Message::getPayload)
                .collect(toList()));
    }

    @Override
    public void consume(MessageConsumer<T> consume) throws Exception {
        consumer.consume(consume);
    }

    @Override
    public void startup() throws Exception { }

    @Override
    public void shutdown() throws Exception {
        producer.shutdown();
        consumer.shutdown();
    }
}
