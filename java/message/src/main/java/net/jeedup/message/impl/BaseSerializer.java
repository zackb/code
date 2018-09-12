package net.jeedup.message.impl;

import net.jeedup.message.Serde;

/**
 * Created by zack on 9/11/18.
 */
public abstract class BaseSerializer<T> implements Serde.Serializer<T> {

    protected Class<T> clazz;

    public BaseSerializer(Class<T> clazz) {
        this.clazz = clazz;
    }
}
