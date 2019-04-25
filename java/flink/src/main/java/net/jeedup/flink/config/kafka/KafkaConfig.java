package net.jeedup.flink.config.kafka;


import net.jeedup.flink.config.SourceConfig;
import net.jeedup.flink.config.TargetConfig;

import java.io.Serializable;

import static net.jeedup.flink.config.FlinkConfig.Format;


/**
 * Kafka config vars.
 *
 * Created by zack on 3/21/19.
 */
public class KafkaConfig implements SourceConfig, TargetConfig, Serializable {

    // kafka cluster read or write to, in bootstrap.servers form
    public String bootstrapServers;

    // kafka topic to read or write to
    public String topic;

    // data format
    private Format format;

    // consumer group.id
    public String groupId;

    @Override
    public String sourceName() {
        return "kafka-" + topic;
    }

    @Override
    public String targetName() {
        return "kafka-" + topic;
    }

    @Override
    public Format getFormat() {
        return format;
    }

    @Override
    public void setFormat(Format format) {
        this.format = format;
    }
}
