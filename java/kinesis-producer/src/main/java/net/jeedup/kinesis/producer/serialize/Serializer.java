package net.jeedup.kinesis.producer.serialize;

import java.nio.ByteBuffer;

/**
 * Simple serialization interface.
 *
 * Created by zack on 4/17/18.
 */
public interface Serializer<T> {

    /**
     * Serialize an object to a byte buffer
     * @param object to serialize
     * @return bytes representing the serialized object
     */
    ByteBuffer serialize(T object);

    /**
     * Deserialized an object from bytes created with {@link #serialize(Object)}
     * @param bytes to deserialize
     * @return object representation of the bytes
     */
    T deserialize(ByteBuffer bytes);

    /**
     * Extract the partition key that should be used when writing this object to Kinesis
     * @param t to get partition key for
     * @return partition key
     */
    String partitionKey(T t);
}
