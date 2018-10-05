package net.jeedup.kafka.producer;

import net.jeedup.kafka.KafkaConfig;
import org.apache.kafka.clients.producer.Producer;
import org.apache.kafka.clients.producer.ProducerConfig;
import org.apache.kafka.clients.producer.ProducerRecord;
import org.apache.kafka.common.serialization.StringSerializer;

import java.util.Collection;
import java.util.Properties;
import java.util.logging.Level;
import java.util.logging.Logger;

import static net.jeedup.message.broker.BrokerUtil.createClientId;

/**
 * A simple Kafka producer
 *
 * Created by zack on 10/3/18.
 */
public class KafkaProducer<T> {

    private static final Logger log = Logger.getLogger(KafkaProducer.class.getSimpleName());

    private final String topic;

    private final Producer<String, T> producer;

    public KafkaProducer(final KafkaConfig config) {

        topic = config.topic;

        Properties props = new Properties();
        props.put(ProducerConfig.CLIENT_ID_CONFIG, createClientId());
        props.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, config.bootstrapServerString());


        if (config.exactlyOnce) {
            props.put(ProducerConfig.ENABLE_IDEMPOTENCE_CONFIG, true);
        } else {
            props.put(ProducerConfig.ACKS_CONFIG, config.acks);
            props.put(ProducerConfig.RETRIES_CONFIG, config.retries);
        }

        props.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG, StringSerializer.class.getName());
        props.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG, config.serdeClassName);

        producer = new org.apache.kafka.clients.producer.KafkaProducer<>(props);
    }

    public void publish(T msg) {

        // TODO: partition key?
        ProducerRecord<String, T> record = new ProducerRecord<>(topic, msg);

        long start = System.currentTimeMillis();

        producer.send(record, (metadata, exception) -> {
            long elapsedTime = System.currentTimeMillis() - start;
            if (metadata != null) {
                log.finest(String.format("sent record(key=%s value=%s) meta(partition=%d, offset=%d) time=%d",
                        record.key(), record.value(), metadata.partition(),
                        metadata.offset(), elapsedTime));
            }
            if (exception != null) {
                log.log(Level.SEVERE, "Failed sending message to kafka", exception);
            }
        });
    }

    public void publish(Collection<T> messages) {
        for (T message : messages) {
            publish(message);
        }
    }

    public void shutdown() {
        if (producer != null) {
            producer.close();
        }
    }
}
