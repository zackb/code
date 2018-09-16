package net.jeedup.kinesis.util;

import com.amazonaws.ClientConfiguration;
import com.amazonaws.Protocol;
import com.amazonaws.auth.AWSCredentialsProvider;
import com.amazonaws.auth.DefaultAWSCredentialsProviderChain;
import com.amazonaws.services.kinesis.AmazonKinesis;
import com.amazonaws.services.kinesis.AmazonKinesisClientBuilder;
import com.amazonaws.services.kinesisfirehose.AmazonKinesisFirehose;
import com.amazonaws.services.kinesisfirehose.AmazonKinesisFirehoseClientBuilder;

/**
 * Utility methods for working with Kinesis Streams
 * Created by zack on 4/16/18.
 */
public class KinesisUtil {

    // default timeouts
    public static final int DEFAULT_CONNECT_TIMEOUT    = 10000;
    public static final int DEFAULT_SOCKET_TIMEOUT     = 60000;

    /**
     * Constructs a Kinesis Stream Client with default timesouts
     * @return Kinesis Stream client
     */
    public static AmazonKinesis buildKinesisStreamClient() {
        return buildKinesisStreamClient(DEFAULT_CONNECT_TIMEOUT, DEFAULT_SOCKET_TIMEOUT);
    }

    /**
     * Constructs a Kinesis Stream Client with the given timeouts
     * @param connectTimeout connect timeout
     * @param socketTimeout socket timeout
     * @return Kinesis Stream client
     */
    public static AmazonKinesis buildKinesisStreamClient(int connectTimeout, int socketTimeout) {
        AWSCredentialsProvider provider = new DefaultAWSCredentialsProviderChain();
        ClientConfiguration kinesisConfig = new ClientConfiguration();
        kinesisConfig.setProtocol(Protocol.HTTPS);
        kinesisConfig.setConnectionTimeout(connectTimeout);
        kinesisConfig.setSocketTimeout(socketTimeout);
        return AmazonKinesisClientBuilder
                .standard()
                .withClientConfiguration(kinesisConfig)
                .withCredentials(provider)
                .build();
    }

    /**
     * Construct a new Kinesis Firehose client
     * @return new Firehose client
     */
    public static AmazonKinesisFirehose buildKinesisFirehoseClient() {
        return buildKinesisFirehoseClient(DEFAULT_CONNECT_TIMEOUT, DEFAULT_SOCKET_TIMEOUT);
    }

    /**
     * Construct a new Kinesis Firehose client
     * @param connectTimeout connect timeout
     * @param socketTimeout socket timeout
     * @return new Firehose client
     */
    public static AmazonKinesisFirehose buildKinesisFirehoseClient(int connectTimeout, int socketTimeout) {
        AWSCredentialsProvider provider = new DefaultAWSCredentialsProviderChain();
        ClientConfiguration firehoseConfig = new ClientConfiguration();
        firehoseConfig.setMaxConnections(1);
        firehoseConfig.setProtocol(Protocol.HTTPS);
        firehoseConfig.setConnectionTimeout(connectTimeout);
        firehoseConfig.setSocketTimeout(socketTimeout);
        return AmazonKinesisFirehoseClientBuilder
                .standard()
                .withClientConfiguration(firehoseConfig)
                .withCredentials(provider)
                .build();
    }
}
