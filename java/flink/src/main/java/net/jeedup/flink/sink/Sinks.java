package net.jeedup.flink.sink;

import net.jeedup.common.json.JSON;
import net.jeedup.flink.codec.JsonEncoder;
import net.jeedup.flink.codec.ParquetWriterSink;
import net.jeedup.flink.config.FlinkConfig;
import net.jeedup.flink.config.fs.FileSystemConfig;
import net.jeedup.flink.config.kafka.KafkaConfig;
import net.jeedup.flink.config.kinesis.KinesisConfig;
import net.jeedup.flink.sink.bucketassigners.ConfigDateBucketAssigner;
import org.apache.flink.core.fs.Path;
import org.apache.flink.formats.parquet.ParquetWriterFactory;
import org.apache.flink.streaming.api.functions.sink.SinkFunction;
import org.apache.flink.streaming.api.functions.sink.filesystem.StreamingFileSink;
import org.apache.flink.streaming.api.functions.sink.filesystem.rollingpolicies.OnCheckpointRollingPolicy;
import org.apache.flink.streaming.connectors.kafka.FlinkKafkaProducer;
import org.apache.kafka.clients.producer.ProducerConfig;

import java.util.Properties;


/**
 * Utility to wrap Flink sink creation
 *
 * Created by zack on 3/19/19.
 */
public class Sinks {

    /**
     * Create a Flink SinkFunction appropriate for the config.
     * @param config with a target defined
     * @return sink function writing to FS (s3), Kafka, or Kinesis
     */
    public static <T> SinkFunction<T> createSink(FlinkConfig<T> config) {

        if (config.target == null)
            throw new RuntimeException("No target defined");

        SinkFunction<T> sink = null;

        if (config.target instanceof FileSystemConfig) {
            switch (config.target.getFormat()) {
                case JSON:
                    sink = createJsonFileSink(config);
                    break;
                case PARQUET:
                    sink = createParquetFileSink(config);
                    break;
                default:
                    throw new RuntimeException("No target format defined");
            }
        } else if (config.target instanceof KafkaConfig) {
            sink = createKafkaSink(config);
        } else if (config.target instanceof KinesisConfig) {
            sink = createKinesisSink(config);
        }

        return sink;
    }

    /**
     * Create a writer using reflection for avro schema discovery.
     * @return sink which will write data as parquet/snappy partitioned
     */
    public static <T> SinkFunction<T> createParquetFileSink(FlinkConfig<T> config) {

        FileSystemConfig fsc = (FileSystemConfig)config.target;

        ParquetWriterFactory<T> parquetWriterFactory = ParquetWriterSink.forReflectRecord(config.dataType);
        return StreamingFileSink.forBulkFormat(
                new Path(fsc.path + "/parquet/" + config.dataTypeName()),
                parquetWriterFactory)
                .withBucketAssigner(new ConfigDateBucketAssigner<>())
                .build();
    }

    public static <T> SinkFunction<T> createJsonFileSink(FlinkConfig<T> config) {
        FileSystemConfig fsc = (FileSystemConfig)config.target;
        return StreamingFileSink
                .forRowFormat(
                        new Path(fsc.path + "/json/" + config.dataTypeName()),
                        new JsonEncoder<T>())
                .withBucketAssigner(new ConfigDateBucketAssigner<>())
                .withRollingPolicy(OnCheckpointRollingPolicy.build())
                .build();
    }

    /**
     * Crate a kafka sink writing to the target kafka cluster defined in the config
     * @param config application config
     * @return sink function writing to kafka
     */
    public static <T> SinkFunction<T> createKafkaSink(FlinkConfig<T> config) {

        KafkaConfig kc = (KafkaConfig)config.target;

        Properties props = new Properties();
        props.setProperty(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, kc.bootstrapServers);
        props.setProperty(ProducerConfig.BATCH_SIZE_CONFIG, "16384");
        // props.setProperty(ProducerConfig.REQUEST_TIMEOUT_MS_CONFIG, "70000");
        props.setProperty(ProducerConfig.LINGER_MS_CONFIG, "1");
        //props.setProperty(ProducerConfig.COMPRESSION_TYPE_CONFIG, "snappy");
        props.setProperty(ProducerConfig.BUFFER_MEMORY_CONFIG, "33554432");

        return new FlinkKafkaProducer<>(kc.topic, JSON::encodeBytes, props);
    }

    /**
     * Crate a kinesis sink writing to the target kinesis stream defined in the config
     * @param config application config
     * @return sink function writing to kinesis
     */
    public static <T> SinkFunction<T> createKinesisSink(FlinkConfig<T> config) {

        KinesisConfig kc = (KinesisConfig)config.target;

        return new JeedupKinesisProducer<>(kc.streamName, config.aws);
    }
}
