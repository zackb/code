package net.jeedup.kinesis.producer.serialize.impl;

import net.jeedup.common.json.JSON;
import net.jeedup.kinesis.producer.serialize.Serializer;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * JSON Serializer implementation. Users need to implement the {@link Serializer#partitionKey(Object)}
 * Created by zack on 4/17/18.
 */
public abstract class JSONSerializer<T> implements Serializer<T> {

    private static final Logger log = Logger.getLogger(JSONSerializer.class.getSimpleName());

    public static final String CHARSET = "UTF-8";

    private final Class<T> clazz;

    public JSONSerializer(Class<T> clazz) {
        this.clazz = clazz;
    }

    @Override
    public ByteBuffer serialize(T object) {
        // need to append a newline here because some AWS services
        // (Athena, Spectrum) expect one record per line. Kinesis Analytics dont care
        String data = JSON.encode(object) + "\n";
        try {
            return ByteBuffer.wrap(data.getBytes(CHARSET));
        } catch (UnsupportedEncodingException e) {
            log.log(Level.SEVERE, "Failed serializing object: " + object, e);
        }
        return null;
    }

    @Override
    public T deserialize(ByteBuffer bytes) {
        return JSON.decodeObject(bytes.array(), clazz);
    }
}
