package net.jeedup.flink.config;

import static net.jeedup.flink.config.FlinkConfig.Format;

/**
 * Marker interface for target configuration. Where to write data to.
 *
 * Created by zack on 3/21/19.
 */
public interface TargetConfig {

    /**
     * A human readable name to call this target. Used in UI
     * @return name of target
     */
    String targetName();

    /**
     * The format which data should be written.
     * @return parquet or json
     */
    Format getFormat();

    /**
     * Set the format for which data should be written
     * @param format parquet or json
     */
    void setFormat(Format format);
}
