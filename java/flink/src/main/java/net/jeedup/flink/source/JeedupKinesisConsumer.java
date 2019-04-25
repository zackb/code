package net.jeedup.flink.source;

import net.jeedup.common.json.JSON;
import net.jeedup.flink.config.aws.AwsConfig;
import org.apache.flink.api.common.serialization.AbstractDeserializationSchema;
import org.apache.flink.api.common.serialization.DeserializationSchema;
import org.apache.flink.streaming.connectors.kinesis.FlinkKinesisConsumer;
import org.apache.flink.streaming.connectors.kinesis.config.ConsumerConfigConstants;

import java.io.IOException;
import java.util.Properties;

import static net.jeedup.common.util.Util.empty;

/**
 * Provides a source stream function reading from a Kinesis Stream.
 * Data type of the stream is templatizable.
 * Created by zack on 1/29/19.
 */
public class JeedupKinesisConsumer<T> extends FlinkKinesisConsumer<T> {

    public JeedupKinesisConsumer(Class<T> clazz, String streamName, AwsConfig awsConfig) {
        super(streamName, buildDeserialzationSchema(clazz), buildProperties(awsConfig));
    }

    private static Properties buildProperties(AwsConfig awsConfig) {
        Properties props = new Properties();

        if (!empty(awsConfig.region))
            props.put(ConsumerConfigConstants.AWS_REGION, awsConfig.region);

        if (empty(awsConfig.accessKey)) {
            props.put(ConsumerConfigConstants.AWS_CREDENTIALS_PROVIDER, "AUTO"); //AWSConfigConstants.CredentialProvider.AUTO);
        } else {
            if (!empty(awsConfig.accessKey))
                props.put(ConsumerConfigConstants.AWS_ACCESS_KEY_ID, awsConfig.accessKey);

            if (!empty(awsConfig.secretKey))
                props.put(ConsumerConfigConstants.AWS_SECRET_ACCESS_KEY, awsConfig.secretKey);
        }

        props.put(ConsumerConfigConstants.STREAM_INITIAL_POSITION, "LATEST"); // ??

        return props;
    }

    private static <T> DeserializationSchema<T> buildDeserialzationSchema(Class<T> clazz) {
        return new AbstractDeserializationSchema<T>(clazz) {
            @Override
            public T deserialize(byte[] message) throws IOException {
                return JSON.decodeObject(message, clazz);
            }
        };
    }
}
