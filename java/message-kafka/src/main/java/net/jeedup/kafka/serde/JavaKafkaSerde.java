package net.jeedup.kafka.serde;

import net.jeedup.message.impl.JavaSerde;

/**
 * Simple Java serialization + Kafka serde. Probably don't want to use this.
 *
 * Created by zack on 10/3/18.
 */
public class JavaKafkaSerde<T> extends KafkaSerde<T> {

    public JavaKafkaSerde() {
        super(new JavaSerde<>());
    }
}
