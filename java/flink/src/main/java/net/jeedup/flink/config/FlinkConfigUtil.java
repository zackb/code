package net.jeedup.flink.config;

import net.jeedup.flink.config.aws.AwsConfig;
import net.jeedup.flink.config.fs.FileSystemConfig;
import net.jeedup.flink.config.kafka.KafkaConfig;
import net.jeedup.flink.config.kinesis.KinesisConfig;
import org.apache.flink.api.java.utils.ParameterTool;

import java.util.logging.Logger;

import static net.jeedup.common.util.StringUtil.capitalize;
import static net.jeedup.common.util.Util.empty;

/**
 * Wrangle config sources. Environment variables, System Properties, Program Arguments, Cluster Setting
 *
 * Created by zack on 3/21/19.
 */
public class FlinkConfigUtil {

    private static final Logger log = Logger.getLogger(FlinkConfigUtil.class.getSimpleName());

    /**
     * Create Converter config from program arguments. Will fallback to environment variables,
     * system properties, and then defaults.
     * @param args arguments passed to the program in the form: "--name value"
     * @return config built from anything you can throw at it
     */
    public static <T> FlinkConfig<T> createConfigFromProgramArguments(String[] args) {

        ParameterTool params = ParameterTool.fromArgs(args);


        FlinkConfig<T> config = new FlinkConfig<>();

        // figure out the data type
        String dataTypeName = params.get("data_type");
        config.dataType = findDataType(dataTypeName);

        // checkpoint interval, with defaults
        config.checkpointIntervalSecs = params.getInt("checkpoint_interval", config.checkpointIntervalSecs);

        // buld aws credentials
        config.aws = createAwsConfig(params);

        // build the source's config
        config.source = createSourceConfig(params);

        // build the target's config
        config.target = createTargetConfig(params);

        return config;
    }

    /**
     * Find a Class which matches the config variable "data_type". Will try to prepend net.jeedup.flink.model
     * if the class is not found by the current classloader.
     *
     * @param dataTypeName name of the data model's class
     * @return class for the data type name
     */
    private static <T> Class<T> findDataType(String dataTypeName) {

        if (empty(dataTypeName))
            throw new RuntimeException("Missing required parameter: data_type");

        Class clazz = null;
        try {
            clazz = FlinkConfigUtil.class.getClassLoader().loadClass(dataTypeName);
        } catch (ClassNotFoundException e) {
            log.warning("Can not find class: " + dataTypeName + " trying the model package");
        }

        if (clazz == null) {
            String className = "net.jeedup.flink.model." + capitalize(dataTypeName);
            try {
                clazz = FlinkConfigUtil.class.getClassLoader().loadClass(className);
            } catch (ClassNotFoundException e) {
                throw new RuntimeException("Can not find model class for data_type: " + dataTypeName, e);
            }
        }

        return clazz;
    }


    /**
     * Create a source config given the program arguments.
     * @param params containing one of filesystem, kafka, or kinesis config values
     * @return source config to use given the arguments
     */
    private static SourceConfig createSourceConfig(ParameterTool params) {
        SourceConfig config = null;
        if (!empty(params.get("source_file_path"))) {
            config = createSourceFileSystemConfig(params);
        } else if (!empty(params.get("source_kafka_topic"))) {
            config = createSourceKafkaConfig(params);
        } else if (!empty(params.get("source_kinesis_stream_name"))) {
            config = createSourceKinesisConfig(params);
        }

        return config;
    }

    /**
     * Create a target config given the program arguments.
     * @param params containing one of filesystem, kafka, or kinesis config values
     * @return target config to use given the arguments
     */
    private static TargetConfig createTargetConfig(ParameterTool params) {
        TargetConfig config = null;
        if (!empty(params.get("target_file_path"))) {
            config = createTargetFileSystemConfig(params);
        } else if (!empty(params.get("target_kafka_topic"))) {
            config = createTargetKafkaConfig(params);
        } else if (!empty(params.get("target_kinesis_stream_name"))) {
            config = createTargetKinesisConfig(params);
        }

        return config;
    }

    /**
     * Create a source which polls a filesystem endpoint.
     * @param params containing filesystem source config parameters
     * @return file system source config
     */
    private static FileSystemConfig createSourceFileSystemConfig(ParameterTool params) {
        FileSystemConfig config = new FileSystemConfig();
        config.path = params.get("source_file_path");
        String format = params.get("source_file_format");
        if (!empty(format)) {
            if ("json".equalsIgnoreCase(format))
                config.setFormat(FlinkConfig.Format.JSON);
            else
                config.setFormat(FlinkConfig.Format.PARQUET);
        }
        config.pollingIntervalSeconds = params.getInt("source_file_poll_interval", config.pollingIntervalSeconds);

        config.lowerBound = params.get("source_file_lower_bound");
        config.higherBound = params.get("source_file_higher_bound");
        return config;
    }

    /**
     * Create a target which writes formatted, partitioned data to a filesystem
     * @param params containing filesystem target config parameters
     * @return file system target config
     */
    private static FileSystemConfig createTargetFileSystemConfig(ParameterTool params) {
        FileSystemConfig config = new FileSystemConfig();
        config.path = params.get("target_file_path");
        String format = params.get("target_file_format");
        if (!empty(format)) {
            if ("json".equalsIgnoreCase(format))
                config.setFormat(FlinkConfig.Format.JSON);
            else
                config.setFormat(FlinkConfig.Format.PARQUET);
        }
        return config;
    }

    /**
     * Create a source which reads from a Kinesis Data Stream
     * @param params containing kinesis stream name to read from
     * @return kinesis source config
     */
    private static KinesisConfig createSourceKinesisConfig(ParameterTool params) {
        KinesisConfig config = new KinesisConfig();
        config.streamName = params.get("source_kinesis_stream_name");
        return config;
    }

    /**
     * Create a target which writes data to a Kinesis Data Stream
     * @param params containing kinesis stream name to write to
     * @return kinesis target config
     */
    private static KinesisConfig createTargetKinesisConfig(ParameterTool params) {
        KinesisConfig config = new KinesisConfig();
        config.streamName = params.get("target_kinesis_stream_name");
        return config;
    }

    /**
     * Create a source which reads data from a Kafka cluster
     * @param params containing kafka boostrap.servers string and a topic name
     * @return kafka source config
     */
    private static KafkaConfig createSourceKafkaConfig(ParameterTool params) {
        KafkaConfig config = new KafkaConfig();
        config.bootstrapServers = params.get("source_kafka_bootstrap_servers");
        config.topic = params.get("source_kafka_topic");
        return config;
    }

    /**
     * Create a target which writes data to a Kafka cluster
     * @param params containing kafka boostrap.servers string and a topic name
     * @return kafka target config
     */
    private static KafkaConfig createTargetKafkaConfig(ParameterTool params) {
        KafkaConfig config = new KafkaConfig();
        config.bootstrapServers = params.get("target_kafka_bootstrap_servers");
        config.topic = params.get("target_kafka_topic");
        return config;
    }

    /**
     * Create AWS credentials from environment variables or program arguments:
     *  Region: aws_region
     *  Access Key: aws_access_key_id
     *  Secret Key: aws_secret_access_key
     * @param params program arguments
     * @return aws config from env vars or program args
     */
    private static AwsConfig createAwsConfig(ParameterTool params) {

        // aws creds are created using env vars by default, can be overriden here
        AwsConfig aws = AwsConfig.createFromEnv();
        if (empty(aws.region))
            aws.region = params.get("aws_region", "us-west-2");
        aws.accessKey = params.get("aws_access_key_id", aws.accessKey);
        aws.secretKey = params.get("aws_secret_access_key", aws.secretKey);
        return aws;
    }
}
