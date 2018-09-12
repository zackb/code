package net.jeedup.message.impl;

import net.jeedup.message.Serde;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Serializer using Java Serailzation
 *
 * Created by zack on 9/12/18.
 */
public class JavaSerializer<T> implements Serde.Serializer<T> {

    private static final Logger log = Logger.getLogger(JavaSerializer.class.getName());

    @Override
    public byte[] serialize(T obj) {

        ByteArrayOutputStream bout = new ByteArrayOutputStream();
        try {
            ObjectOutputStream out = new ObjectOutputStream(bout);
            out.writeObject(obj);
        } catch (IOException e) {
            log.log(Level.SEVERE, "Failed serializing object", e);
        }
        return bout.toByteArray();
    }
}
