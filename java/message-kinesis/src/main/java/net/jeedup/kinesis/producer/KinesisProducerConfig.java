package net.jeedup.kinesis.producer;

import net.jeedup.kinesis.KinesisConfig;
import net.jeedup.kinesis.producer.serialize.Serializer;

/**
 * Producer config, some variables specific to particular servce (stream, firehose, kpl)
 * Created by zack on 9/15/18.
 */
public class KinesisProducerConfig extends KinesisConfig {

    public Serializer<?> serializer;

    // the *target* amount of time to buffer messages in the KPL before flushing to AWS
    public long maxBufferedTimeMillis = 500L;

    // the maximum number of records the KPL may aggregate before flushing to AWS
    public long maxAggregationCount = 4294967295L;
}
