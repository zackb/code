package net.jeedup.kinesis.util;

import com.amazonaws.ClientConfiguration;
import com.amazonaws.Protocol;
import com.amazonaws.auth.AWSCredentialsProvider;
import com.amazonaws.auth.DefaultAWSCredentialsProviderChain;
import com.amazonaws.services.kinesis.AmazonKinesis;
import com.amazonaws.services.kinesis.AmazonKinesisClientBuilder;
import com.amazonaws.services.kinesis.clientlibrary.types.UserRecord;
import com.amazonaws.services.kinesisfirehose.AmazonKinesisFirehose;
import com.amazonaws.services.kinesisfirehose.AmazonKinesisFirehoseClientBuilder;
import com.amazonaws.services.lambda.runtime.events.KinesisEvent;

import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;

/**
 * Utility methods for working with Kinesis Streams
 * Created by zack on 4/16/18.
 */
public class KinesisUtil {

    // default timeouts
    public static final int DEFAULT_CONNECT_TIMEOUT    = 10000;
    public static final int DEFAULT_SOCKET_TIMEOUT     = 60000;

    public static final String CHARSET = "UTF-8";
    public static final CharsetDecoder decoder = Charset.forName(CHARSET).newDecoder();

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

    /**
     * Method to bulk deaggregate a set of Kinesis user records from a list of
     * Kinesis Event Records.
     *
     * @param inputRecords
     *            The Kinesis Event Records provided by AWS Lambda
     * @return A list of Kinesis UserRecord objects obtained by deaggregating
     *         the input list of KinesisEventRecords
     */
    public static List<UserRecord> deaggregate(List<KinesisEvent.KinesisEventRecord> inputRecords) {
        List<UserRecord> outputRecords = new LinkedList<>();
        for (KinesisEvent.KinesisEventRecord inputRecord : inputRecords) {
            outputRecords.addAll(deaggregate(inputRecord));
        }
        return outputRecords;
    }

    /**
     * Method to deaggregate a single Kinesis record into one or more
     * Kinesis user records.
     *
     * @param inputRecord
     *            The single KinesisEventRecord to deaggregate
     * @return A list of Kinesis UserRecord objects obtained by deaggregating
     *         the input KinesisEventRecord
     */
    public static List<UserRecord> deaggregate(KinesisEvent.KinesisEventRecord inputRecord) {
        return UserRecord.deaggregate(Arrays.asList(inputRecord.getKinesis()));
    }
}
