package net.jeedup.flink.sink;

import net.jeedup.common.json.JSON;
import net.jeedup.common.model.Partitionable;
import net.jeedup.flink.config.kafka.KafkaConfig;
import org.apache.flink.streaming.connectors.kafka.FlinkKafkaProducer;
import org.apache.flink.streaming.util.serialization.KeyedSerializationSchema;
import org.apache.kafka.clients.producer.ProducerConfig;

import java.util.Properties;

import static net.jeedup.message.broker.BrokerUtil.createClientId;


/**
 * TODO - Kafka producer/sink
 *
 * Created by zack on 1/10/18.
 */
public class JeedupKafkaProducer<T extends Partitionable> extends FlinkKafkaProducer<T> {

    public JeedupKafkaProducer(KafkaConfig config) {
        super(config.topic,
            new KeyedSerializationSchema<T>() {
                @Override
                public byte[] serializeKey(T element) {
                    return element.partitionKey().getBytes();
                }

                @Override
                public byte[] serializeValue(T element) {
                    return JSON.encodeBytes(element);
                }

                @Override
                public String getTargetTopic(T element) {
                    return config.topic;
                }
            }, buildKafkaProducerProperties(config), FlinkKafkaProducer.Semantic.EXACTLY_ONCE);

        setWriteTimestampToKafka(true);
    }

    private static Properties buildKafkaProducerProperties(KafkaConfig config) {
        Properties props = new Properties();
        props.setProperty(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, config.bootstrapServers);
        props.put(ProducerConfig.CLIENT_ID_CONFIG, createClientId());
        props.put(ProducerConfig.TRANSACTION_TIMEOUT_CONFIG, "30000");
        props.setProperty(ProducerConfig.BATCH_SIZE_CONFIG, "16384");
        // props.setProperty(ProducerConfig.REQUEST_TIMEOUT_MS_CONFIG, "70000");
        props.setProperty(ProducerConfig.LINGER_MS_CONFIG, "1");
        //props.setProperty(ProducerConfig.COMPRESSION_TYPE_CONFIG, "snappy");
        props.setProperty(ProducerConfig.BUFFER_MEMORY_CONFIG, "33554432");
        return props;
    }

}
