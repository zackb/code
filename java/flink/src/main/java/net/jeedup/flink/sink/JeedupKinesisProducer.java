package net.jeedup.flink.sink;

import net.jeedup.common.json.JSON;
import net.jeedup.common.util.UUIDs;
import net.jeedup.flink.config.aws.AwsConfig;
import org.apache.flink.streaming.connectors.kinesis.FlinkKinesisProducer;
import org.apache.flink.streaming.connectors.kinesis.KinesisPartitioner;
import org.apache.flink.streaming.connectors.kinesis.config.AWSConfigConstants;

import java.util.Properties;

import static net.jeedup.common.util.Util.empty;


/**
 * Provides a sink function writing to a Kinesis Stream.
 * Data type of the stream is templatizable.
 *
 * Created by zack on 3/19/19.
 */
public class JeedupKinesisProducer<T> extends FlinkKinesisProducer<T> {

    public JeedupKinesisProducer(String streamName, AwsConfig awsConfig) {
        super(JSON::encodeBytes, buildProperties(awsConfig));
        setDefaultStream(streamName);
        // TODO: you're required a partitioner. Allow clients to specify their own
        setCustomPartitioner(new KinesisPartitioner<T>() {
            @Override
            public String getPartitionId(T t) {
                return UUIDs.random().toString();
            }
        });
    }

    private static Properties buildProperties(AwsConfig awsConfig) {
        Properties props = new Properties();

        // disable aggregation if it's not supported by a consumer!!!
        // props.put("AggregationEnabled", "false");

        if (!empty(awsConfig.region))
            props.put(AWSConfigConstants.AWS_REGION, awsConfig.region);

        if (empty(awsConfig.accessKey)) {
            props.put(AWSConfigConstants.AWS_CREDENTIALS_PROVIDER, "AUTO");
        } else {
            if (!empty(awsConfig.accessKey))
                props.put(AWSConfigConstants.AWS_ACCESS_KEY_ID, awsConfig.accessKey);

            if (!empty(awsConfig.secretKey))
                props.put(AWSConfigConstants.AWS_SECRET_ACCESS_KEY, awsConfig.secretKey);
        }

        return props;
    }
}
