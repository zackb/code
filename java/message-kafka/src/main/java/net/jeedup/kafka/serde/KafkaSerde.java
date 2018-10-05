package net.jeedup.kafka.serde;

import net.jeedup.message.Serde;
import org.apache.kafka.common.serialization.Deserializer;
import org.apache.kafka.common.serialization.Serializer;

import java.util.Map;

/**
 * Reuse functionality from {@link net.jeedup.message.Serde} for Kafka Serializers and Deserializers
 *
 * Created by zack on 10/3/18.
 */
public class KafkaSerde<T> implements Deserializer<T>, Serializer<T> {

    private final Serde<T> serde;

    public KafkaSerde(Serde<T> serde) {
        this.serde = serde;
    }

    @Override
    public void configure(Map<String, ?> configs, boolean isKey) { }

    @Override
    public byte[] serialize(String topic, T data) {
        return serde.serialize(data);
    }

    @Override
    public T deserialize(String topic, byte[] data) {
        return serde.deserialize(data);
    }

    @Override
    public void close() { }
}
