package net.jeedup.message;

/**
 * Serialize and deserialize messages
 *
 * Created by zack on 9/11/18.
 */
public interface Serde<T> {

    byte[] serialize(T obj);

    T deserialize(byte[] bytes);

    interface Serializer<T> {
        byte[] serialize(T obj);
    }

    interface Deserializer<T> {
        T deserialize(byte[] bytes);
    }
}
