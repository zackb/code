package net.jeedup.flink;

import net.jeedup.flink.config.FlinkConfig;
import net.jeedup.flink.config.FlinkConfigUtil;
import net.jeedup.flink.sink.Sinks;
import net.jeedup.flink.source.Sources;
import net.jeedup.flink.source.timestamps.JeedupBoundedOutOfOrdernessTimestampAssigner;
import org.apache.flink.streaming.api.CheckpointingMode;
import org.apache.flink.streaming.api.TimeCharacteristic;
import org.apache.flink.streaming.api.datastream.DataStream;
import org.apache.flink.streaming.api.environment.StreamExecutionEnvironment;
import org.apache.flink.streaming.api.windowing.time.Time;


/**
 * Utility to move data from one place/format to another place/format.
 * For instance, to translate data from json/gzip (Firehose) to partitioned parquet/snappy in S3
 *
 * Created by zack on 3/19/19.
 */
public class Converter<T> {

    private final StreamExecutionEnvironment env;
    private final FlinkConfig<T> config;

    public Converter(FlinkConfig<T> config) {
        this.config = config;

        env = StreamExecutionEnvironment.getExecutionEnvironment();

        // use event time from kafka for the time characteristic
        env.setStreamTimeCharacteristic(TimeCharacteristic.EventTime);

        // checkpoint exactly-once with the specified frequency
        env.enableCheckpointing(1000 * config.checkpointIntervalSecs, CheckpointingMode.EXACTLY_ONCE);
    }

    public void start() throws Exception {

        String name = config.dataTypeName().toLowerCase();

        DataStream<T> stream = Sources.createSource(config, env)
                .name(config.source.sourceName() + "-source")
                .uid(config.source.sourceName() + "-source-id")
                // .rescale()
                .assignTimestampsAndWatermarks(new JeedupBoundedOutOfOrdernessTimestampAssigner<>(Time.seconds(5)));

        stream.addSink(Sinks.createSink(config))
                .name(config.target.targetName() + "-sink")
                .uid(config.target.targetName() + "-sink-id");

        env.execute(name + "-convert");
    }

    // --data_type MyModel --checkpoint_interval 900 --aws_region us-west-2 --aws_access_key fff --aws_secret_access_key fdsfdsf --source_file_path s3a://bucket/json/2019/03/21 --source_file_poll_interval 900 --target_file_path s3a://bucket/flink/job --target_file_format parquet
    public static void main(String[] args) throws Exception {

        FlinkConfig<?> config = FlinkConfigUtil.createConfigFromProgramArguments(args);

        new Converter<>(config).start();
    }
}
