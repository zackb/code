package net.jeedup.kinesis.producer.impl;

import com.amazonaws.services.kinesis.AmazonKinesis;
import com.amazonaws.services.kinesis.model.PutRecordsRequest;
import com.amazonaws.services.kinesis.model.PutRecordsRequestEntry;
import com.amazonaws.services.kinesis.model.PutRecordsResult;
import com.amazonaws.services.kinesis.model.PutRecordsResultEntry;
import net.jeedup.kinesis.producer.KinesisProducer;
import net.jeedup.kinesis.producer.serialize.Serializer;
import net.jeedup.kinesis.util.KinesisUtil;

import java.util.*;
import java.util.logging.Level;
import java.util.logging.Logger;

import static net.jeedup.kinesis.util.KinesisUtil.buildKinesisStreamClient;

/**
 * Utility to wrap the Kinesis Stream client's producer functionality to do common things like:
 *  - Marshall records
 *  - Deaggregate records
 *  - Handle Kinesis limits (500 records per PUT, 5MB per PUT)
 *  - Handle errors and retries
 *
 * Note: This is NOT the KPL and should only be used if the KPL is not an option
 *
 * Created by zack on 4/17/18.
 */
public class StreamProducer<T> extends BatchingProducer<T> implements KinesisProducer<T> {

    private static final Logger log = Logger.getLogger(StreamProducer.class.getSimpleName());

    private final Set<String> streamNames;
    protected final Serializer<T> serializer;
    protected final AmazonKinesis kinesis;

    /**
     * Create a new Kinesis Stream producer wrapper. This will handle batch size limits and failure retry.
     * @param streamName name of the Kinesis Stream to write to
     * @param serializer utility to encode items to Kinesis friendly formats
     */
    public StreamProducer(String streamName, Serializer<T> serializer) {
        this(Collections.singleton(streamName), serializer);
    }

    /**
     * Create a new Kinesis Stream producer wrapper. This will handle batch size limits and failure retry.
     * @param streamNames each Kinesis Stream name to write to
     * @param serializer utility to encode items to Kinesis friendly formats
     */
    public StreamProducer(Set<String> streamNames, Serializer<T> serializer) {
        this(streamNames, serializer, KinesisUtil.DEFAULT_CONNECT_TIMEOUT, KinesisUtil.DEFAULT_SOCKET_TIMEOUT);
    }

    /**
     * Create a new Kinesis Stream producer wrapper. This will handle batch size limits and failure retry.
     * @param streamName name of the Kinesis Stream to write to
     * @param serializer utility to encode items to Kinesis friendly formats
     * @param connectTimeout number of milliseconds to wait to connect to Kinesis
     * @param socketTimeout number of milliseconds to wait on the socket write to finish
     */
    public StreamProducer(String streamName, Serializer<T> serializer, int connectTimeout, int socketTimeout) {
        this(Collections.singleton(streamName), serializer, connectTimeout, socketTimeout);
    }

    /**
     * Create a new Kinesis Stream producer wrapper. This will handle batch size limits and failure retry.
     * @param streamNames each Kinesis Stream name to write to
     * @param serializer utility to encode items to Kinesis friendly formats
     * @param connectTimeout number of milliseconds to wait to connect to Kinesis
     * @param socketTimeout number of milliseconds to wait on the socket write to finish
     */
    public StreamProducer(Set<String> streamNames, Serializer<T> serializer, int connectTimeout, int socketTimeout) {
        this.streamNames = streamNames;
        this.serializer = serializer;
        this.kinesis = buildKinesisStreamClient(connectTimeout, socketTimeout);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void putRecordsBatch(Collection<T> records) {

        // create the initial list of PUT requests
        List<PutRecordsRequestEntry> puts = new ArrayList<>(records.size());

        for (T record : records) {
            // make each record a PUT request and ask the serializer to get the partiion key and serialization
            puts.add(new PutRecordsRequestEntry()
                    .withPartitionKey(serializer.partitionKey(record))
                    .withData(serializer.serialize(record)));
        }

        // perform the PUT request on each stream specified
        for (String streamName : streamNames) {

            PutRecordsRequest putRequest = new PutRecordsRequest()
                    .withRecords(puts)
                    .withStreamName(streamName);

            PutRecordsResult result = kinesis.putRecords(putRequest);

            // check for errors
            while (result.getFailedRecordCount() > 0) {
                log.log(Level.SEVERE, "Got failed records count: " + result.toString());
                List<PutRecordsRequestEntry> failedRecords = new ArrayList<>();
                List<PutRecordsResultEntry> resultEntries = result.getRecords();

                // find the failed records
                for (int i = 0; i < resultEntries.size(); i++) {
                    PutRecordsResultEntry resultEntry = resultEntries.get(i);
                    if (resultEntry.getErrorCode() != null) {
                        failedRecords.add(puts.get(i));
                    }
                }

                // retry puts
                puts = failedRecords;
                putRequest.setRecords(puts);
                result = kinesis.putRecords(putRequest);
            }
        }
    }
}
