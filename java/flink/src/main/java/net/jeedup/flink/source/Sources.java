package net.jeedup.flink.source;

import net.jeedup.flink.config.FlinkConfig;
import net.jeedup.flink.config.fs.FileSystemConfig;
import net.jeedup.flink.config.kafka.KafkaConfig;
import net.jeedup.flink.config.kinesis.KinesisConfig;
import net.jeedup.flink.source.io.JsonReflectInputFormat;
import net.jeedup.flink.util.Util;
import org.apache.flink.api.common.io.FilePathFilter;
import org.apache.flink.core.fs.Path;
import org.apache.flink.streaming.api.datastream.DataStreamSource;
import org.apache.flink.streaming.api.functions.source.FileProcessingMode;
import org.apache.flink.streaming.api.environment.StreamExecutionEnvironment;
import org.apache.flink.streaming.api.functions.source.SourceFunction;
import org.apache.flink.streaming.api.windowing.time.Time;


/**
 * Utility to wrap Flink source creation
 *
 * Created by zack on 3/19/19.
 */
public class Sources {

    /**
     * Create the source stream for this Converter given application config
     * @param config flink config with ONE of the sources defined
     * @param env environment to add source to
     * @return new source stream for the applied config
     */
    public static <T> DataStreamSource<T> createSource(FlinkConfig<T> config, StreamExecutionEnvironment env) {

        if (config.source == null)
            throw new RuntimeException("No source defined");

        DataStreamSource<T> source = null;
        if (config.source instanceof FileSystemConfig) {
            source = createStreamingFileSource(config, env);
        } else if (config.source instanceof KafkaConfig) {
            source = env.addSource(createKafkaSource(config));
        } else if (config.source instanceof KinesisConfig) {
            source = env.addSource(createKinesisSource(config));
        }

        return source;
    }

    /**
     * Create a data stream from file system (s3) data and continuously poll for new files
     *
     * @param config application config with a file system based source
     * @param env the streaming envionment to create the data stream for
     * @return a new Stream of file data which is continuously monitored for new files
     */
    public static <T> DataStreamSource<T> createStreamingFileSource(FlinkConfig<T> config, StreamExecutionEnvironment env) {

        FileSystemConfig fsc = (FileSystemConfig)config.source;

        JsonReflectInputFormat<T> inputFormat = new JsonReflectInputFormat<>(config.dataType, new Path(fsc.path));

        // recurse into sub-"directories"
        inputFormat.setNestedFileEnumeration(true);

        // filter out partial files starting with _
        inputFormat.setFilesFilter(new FilePathFilter() {
            @Override
            public boolean filterPath(Path path) {
                if (path == null)
                    return true;

                boolean bounds = Util.checkBounds(fsc.lowerBound, fsc.higherBound, path.toString());

                return path.getName().startsWith(".") ||
                        path.getName().startsWith("_") ||
                        path.getName().contains("_COPYING_") ||
                        !bounds;
            }
        });

        return env.readFile(inputFormat, fsc.path,
                FileProcessingMode.PROCESS_CONTINUOUSLY,
                Time.seconds(fsc.pollingIntervalSeconds).toMilliseconds());
    }

    /**
     * Create a new Kinesis Source from application config
     * @param config containing the Kinesis Stream source info
     * @return new kinesis source function
     */
    public static <T> SourceFunction<T> createKinesisSource(FlinkConfig<T> config) {
        KinesisConfig kc = (KinesisConfig)config.source;
        return new JeedupKinesisConsumer<>(config.dataType, kc.streamName, config.aws);
    }

    /**
     * Create a new Kafka Source from application config
     * @param config containing the Kafka server and topic source info
     * @return new kafka source function
     */
    public static <T> SourceFunction<T> createKafkaSource(FlinkConfig<T> config) {
        KafkaConfig kc = (KafkaConfig) config.source;
        return new JeedupKafkaConsumer<>(config.dataType, kc.bootstrapServers, kc.topic, kc.groupId);
    }
}
