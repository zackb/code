package net.jeedup.kinesis.producer.impl;

import com.amazonaws.services.kinesisfirehose.AmazonKinesisFirehose;
import com.amazonaws.services.kinesisfirehose.model.PutRecordBatchRequest;
import com.amazonaws.services.kinesisfirehose.model.PutRecordBatchResponseEntry;
import com.amazonaws.services.kinesisfirehose.model.PutRecordBatchResult;
import com.amazonaws.services.kinesisfirehose.model.Record;
import net.jeedup.kinesis.producer.KinesisProducer;
import net.jeedup.kinesis.producer.serialize.Serializer;
import net.jeedup.kinesis.util.KinesisUtil;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

import static java.util.stream.Collectors.toList;
import static net.jeedup.kinesis.util.KinesisUtil.buildKinesisFirehoseClient;

/**
 * Utility to wrap the Kinesis Firehose client's producer functionality to do common things like:
 *  - Marshall records
 *  - Deaggregate records
 *  - Handle limits
 *  - Handle errors and retries
 *
 * Note: This is NOT the KPL and should only be used if the KPL is not an option
 *
 * Created by zack on 4/17/18.
 */
public class FirehoseProducer<T> extends BatchingProducer<T> implements KinesisProducer<T> {

    private static final Logger log = Logger.getLogger(FirehoseProducer.class.getSimpleName());

    private final AmazonKinesisFirehose firehose;
    private final String firehoseName;
    private final Serializer<T> serializer;

    /**
     * Create a new Kinesis Firehose producer wrapper. This will handle batch size limits and failure retry
     * @param firehoseName name of the Kinesis Firehose to write to
     * @param serializer utility to encode items to Kinesis friendly formats
     */
    public FirehoseProducer(String firehoseName, Serializer<T> serializer) {
        this(firehoseName, serializer, KinesisUtil.DEFAULT_CONNECT_TIMEOUT, KinesisUtil.DEFAULT_SOCKET_TIMEOUT);
    }

    /**
     * Create a new Kinesis Firehose producer wrapper. This will handle batch size limits and failure retry
     * @param firehoseName name of the Kinesis Firehose to write to
     * @param serializer utility to encode items to Kinesis friendly formats
     * @param connectTimeout number of milliseconds to wait to connect to Kinesis
     * @param socketTimeout number of milliseconds to wait on the socket write to finish
     */
    public FirehoseProducer(String firehoseName, Serializer<T> serializer, int connectTimeout, int socketTimeout) {
        this.firehoseName = firehoseName;
        this.serializer = serializer;
        this.firehose = buildKinesisFirehoseClient(connectTimeout, socketTimeout);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void putRecordsBatch(Collection<T> records) {
        List<Record> puts = records.stream()
                        .map(e -> new Record().withData(serializer.serialize(e)))
                        .collect(toList());

        PutRecordBatchRequest putRequest = new PutRecordBatchRequest()
                .withRecords(puts)
                .withDeliveryStreamName(firehoseName);

        PutRecordBatchResult result = firehose.putRecordBatch(putRequest);

        while (result.getFailedPutCount() > 0) {

            log.log(Level.SEVERE, "Got failed firehose records count: " + result.toString());
            List<Record> failedRecords = new ArrayList<>();
            List<PutRecordBatchResponseEntry> resultEntries = result.getRequestResponses();

            // find the failed records
            for (int i = 0; i < resultEntries.size(); i++) {
                PutRecordBatchResponseEntry resultEntry = resultEntries.get(i);
                if (resultEntry.getErrorCode() != null) {
                    failedRecords.add(puts.get(i));
                }
            }

            // retry puts
            puts = failedRecords;
            putRequest.setRecords(puts);
            result = firehose.putRecordBatch(putRequest);
        }
    }

    @Override
    public void startup() throws Exception { }

    @Override
    public void shutdown() throws Exception {
        firehose.shutdown();
    }
}
