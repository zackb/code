package net.jeedup.flink.config.kinesis;


import net.jeedup.flink.config.SourceConfig;
import net.jeedup.flink.config.TargetConfig;

import java.io.Serializable;

import static net.jeedup.flink.config.FlinkConfig.Format;

/**
 * AWS Kinesis source/target config vars.
 *
 * Created by zack on 3/21/19.
 */
public class KinesisConfig implements SourceConfig, TargetConfig, Serializable {

    // name of the kinesis data stream (not firehose!)
    public String streamName;

    // data format
    private Format format;

    @Override
    public String sourceName() {
        return "kinesis-" + streamName;
    }

    @Override
    public String targetName() {
        return "kinesis-" + streamName;
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
