package net.jeedup.message.impl;

import net.jeedup.message.Serde;

import java.io.ByteArrayInputStream;
import java.io.ObjectInputStream;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Deserializer using Java Serailzation
 *
 * Created by zack on 9/12/18.
 */
public class JavaDeserializer<T> implements Serde.Deserializer<T> {

    private static final Logger log = Logger.getLogger(JavaDeserializer.class.getName());

    @SuppressWarnings("unchecked")
    @Override
    public T deserialize(byte[] bytes) {
        T obj = null;
        ByteArrayInputStream bin = new ByteArrayInputStream(bytes);
        try {
            ObjectInputStream in = new ObjectInputStream(bin);
            obj = (T)in.readObject();
        } catch (Exception e) {
            log.log(Level.SEVERE, "Failed deserializing object", e);
        }
        return obj;
    }
}
