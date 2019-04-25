package net.jeedup.flink.config;

import net.jeedup.flink.config.aws.AwsConfig;

import java.io.Serializable;

/**
 * Generic flink config for specifying sources and sinks.
 *
 * Created by zack on 3/19/19.
 */
public class FlinkConfig<T> implements Serializable {

    // data formats available to read from and write to
    public enum Format {
        JSON,PARQUET
    }

    // aws config everyone knows and loves
    public AwsConfig aws = AwsConfig.createFromEnv();

    // the type/schema of we are moving
    public Class<T> dataType;

    // how often to checkpoint, in seconds
    public int checkpointIntervalSecs = 60 * 15;

    // where to read data from
    public SourceConfig source;

    // where to write data to
    public TargetConfig target;

    // name of the data type, used in human and machine ways
    public String dataTypeName() {
        return dataType.getSimpleName().toLowerCase();
    }
}
