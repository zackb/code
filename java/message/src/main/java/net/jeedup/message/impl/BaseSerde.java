package net.jeedup.message.impl;

import net.jeedup.message.Serde;

/**
 * Pojo Serde
 *
 * Created by zack on 9/11/18.
 */
public class BaseSerde<T> implements Serde<T> {

    private final Serializer<T> serializer;

    private final Deserializer<T> deserializer;

    public BaseSerde(Serializer<T> serializer, Deserializer<T> deserializer) {
        this.serializer = serializer;
        this.deserializer = deserializer;
    }

    public byte[] serialize(T obj) {
        return serializer.serialize(obj);
    }

    public T deserialize(byte[] bytes) {
        return deserializer.deserialize(bytes);
    }
}
