package net.jeedup.kinesis.consumer;

import net.jeedup.kinesis.KinesisConfig;
import net.jeedup.message.Serde;

/**
 * Consumer config
 * Created by zack on 9/15/18.
 */
public class KinesisConsumerConfig extends KinesisConfig {

    public Serde.Deserializer<?> deserializer;

    // application name to use while reading from kinesis
    // each application gets a full set of data
    public String applicationName;
}
