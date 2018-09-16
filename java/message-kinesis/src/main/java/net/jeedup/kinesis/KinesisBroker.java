package net.jeedup.kinesis;

import net.jeedup.kinesis.consumer.KinesisConsumer;
import net.jeedup.kinesis.consumer.KinesisConsumerConfig;
import net.jeedup.kinesis.producer.KinesisProducer;
import net.jeedup.message.Message;
import net.jeedup.message.broker.MessageBroker;
import net.jeedup.message.broker.MessageConsumer;

import java.util.Collection;

import static java.util.stream.Collectors.toList;

/**
 * Kinesis broker just for kicks, probably would want to use just the
 * {@link net.jeedup.kinesis.consumer.KinesisConsumer} or {@link net.jeedup.kinesis.producer.KinesisProducer}
 * alone for the right use case.
 * Created by zack on 9/15/18.
 */
public class KinesisBroker<T> implements MessageBroker<T> {

    private final KinesisProducer<T> producer;
    private final KinesisConsumer<T> consumer;

    public KinesisBroker(KinesisProducer<T> producer, KinesisConsumerConfig consumerConfig) {
        this.producer = producer;
        this.consumer = new KinesisConsumer<>(consumerConfig);
    }

    @Override
    public void publish(Message<T> message) throws Exception {
        producer.putRecord(message.getPayload());
    }

    @Override
    public void publish(Collection<Message<T>> messages) throws Exception {
        producer.putRecords(messages.stream()
                                .map(Message::getPayload)
                                .collect(toList()));
    }

    @Override
    public void consume(MessageConsumer<T> consume) throws Exception {
        consumer.consume(consume);
    }

    @Override
    public void startup() throws Exception {
        producer.startup();
    }

    @Override
    public void shutdown() throws Exception {
        producer.shutdown();
        consumer.shutdown();
    }
}
