package net.jeedup.flink.config.fs;


import net.jeedup.flink.config.SourceConfig;
import net.jeedup.flink.config.TargetConfig;

import java.io.Serializable;

import static net.jeedup.flink.config.FlinkConfig.Format;

/**
 * Filesystem source/target config vars.
 * S3 locations should use the "s3a" scheme
 *
 * Created by zack on 3/21/19.
 */
public class FileSystemConfig implements SourceConfig, TargetConfig, Serializable {

    // top-level dir in fs/s3 for the target or source
    // target: data will end up in target/<format>/<type>: bueck.flink/flink/parquet/data
    // source: data will be read recursively and polled for updates
    public String path;

    // number of seconds to wait between polling checks for new data
    public int pollingIntervalSeconds = 60 * 15;

    // the alphanum lowest file name to read inclusive (json/2019/01/02)
    public String lowerBound;

    // the alphanum highest file name to read exclusive (json/2019/02/01)
    public String higherBound;

    // data format
    private Format format;

    @Override
    public void setFormat(Format format) {
        this.format = format;
    }

    @Override
    public Format getFormat() {
        return format;
    }

    @Override
    public String sourceName() {
        return name();
    }

    @Override
    public String targetName() {
        return name();
    }

    private String name() {
        return path.startsWith("s3") ? "s3" : "fs";
    }
}
