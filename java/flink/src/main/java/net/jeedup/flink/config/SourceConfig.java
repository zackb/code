package net.jeedup.flink.config;

/**
 * Marker interface for source configuration. Where to read data from.
 *
 * Created by zack on 3/21/19.
 */
public interface SourceConfig {

    /**
     * A human readable name to call this source. Used in UI
     * @return name of target
     */
    String sourceName();
}
