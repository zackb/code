package net.jeedup.message.impl;

import net.jeedup.message.Serde;

/**
 * Created by zack on 9/11/18.
 */
public abstract class BaseDeserializer<T> implements Serde.Deserializer<T> {

    protected Class<T> clazz;

    public BaseDeserializer(Class<T> clazz) {
        this.clazz = clazz;
    }
}
