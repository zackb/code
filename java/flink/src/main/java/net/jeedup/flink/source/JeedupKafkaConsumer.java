package net.jeedup.flink.source;

import net.jeedup.common.json.JSON;
import org.apache.flink.api.common.serialization.AbstractDeserializationSchema;
import org.apache.flink.api.common.serialization.DeserializationSchema;
import org.apache.flink.streaming.connectors.kafka.FlinkKafkaConsumer;
import org.apache.kafka.clients.consumer.ConsumerConfig;

import java.io.IOException;
import java.util.Properties;

import static net.jeedup.message.broker.BrokerUtil.createClientId;

/**
 * Kafka consumer/source
 *
 * Created by zack on 1/8/18.
 */
public class JeedupKafkaConsumer<T> extends FlinkKafkaConsumer<T> {

    public JeedupKafkaConsumer(Class<T> clazz, String bootstrapServers, String topic, String groupId) {
        this(topic, createDesierializationSchema(clazz), buildKafkaConsumerProperties(bootstrapServers, groupId));
    }

    public JeedupKafkaConsumer(String topic, DeserializationSchema<T> valueDeserializer, Properties props) {
        super(topic, valueDeserializer, props);
        setStartFromGroupOffsets();
    }

    private static Properties buildKafkaConsumerProperties(String bootstrapServers, String groupId) {

        Properties props = new Properties();
        props.setProperty(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG, bootstrapServers);
        props.put(ConsumerConfig.CLIENT_ID_CONFIG, createClientId());
        props.put(ConsumerConfig.GROUP_ID_CONFIG, groupId);
        return props;
    }

    private static <T> DeserializationSchema<T> createDesierializationSchema(Class<T> clazz) {
        return new AbstractDeserializationSchema<T>() {
            @Override
            public T deserialize(byte[] message) throws IOException {
                return JSON.decodeObject(message, clazz);
            }
        };
    }
}
