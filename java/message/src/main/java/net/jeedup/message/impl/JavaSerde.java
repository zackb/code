package net.jeedup.message.impl;

/**
 * Java Serialization Serde
 *
 * Created by zack on 9/12/18.
 */
public class JavaSerde<T> extends BaseSerde<T> {

    public JavaSerde() {
        super(new JavaSerializer<>(), new JavaDeserializer<>());
    }
}
