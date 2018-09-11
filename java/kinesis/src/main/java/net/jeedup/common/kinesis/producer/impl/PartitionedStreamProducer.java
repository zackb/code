package net.jeedup.common.kinesis.producer.impl;

import com.amazonaws.services.kinesis.model.PutRecordsRequest;
import com.amazonaws.services.kinesis.model.PutRecordsRequestEntry;
import com.amazonaws.services.kinesis.model.PutRecordsResult;
import com.amazonaws.services.kinesis.model.PutRecordsResultEntry;
import net.jeedup.common.kinesis.producer.KinesisProducer;
import net.jeedup.common.kinesis.producer.serialize.Serializer;

import java.util.*;
import java.util.logging.Level;
import java.util.logging.Logger;

import static net.jeedup.common.util.Util.empty;
import static java.lang.Math.abs;
import static java.util.stream.Collectors.groupingBy;

/**
 * Used to shard/partition a kinesis stream to other streams, or application level sharding.
 *  This will take a collection of streams and write to them using the record's partition key in:
 *      {@link net.jeedup.common.kinesis.producer.serialize.Serializer#partitionKey(Object)}
 *  So if there are two "target streams" defined each will get 50% of the data (assuming an evenly distributed partition key)
 *
 * This also does all the things {@link StreamProducer} does
 *  - Marshall records
 *  - Deaggregate records
 *  - Handle Kinesis limits (500 records per PUT, 5MB per PUT)
 *  - Handle errors and retries
 *
 * Note: This is NOT the KPL and should only be used if the KPL is not an option
 *
 * Created by zack on 9/7/18.
 */
public class PartitionedStreamProducer<T> extends StreamProducer<T> implements KinesisProducer<T> {

    private static final Logger log = Logger.getLogger(PartitionedStreamProducer.class.getSimpleName());

    private static final Random random = new Random();

    private final String[] streams;

    public PartitionedStreamProducer(Set<String> streamNames, Serializer<T> serializer) {
        super(streamNames, serializer);
        streams = streamNames.toArray(new String[0]);
    }

    public PartitionedStreamProducer(String streamName, Serializer<T> serializer, int connectTimeout, int socketTimeout) {
        super(streamName, serializer, connectTimeout, socketTimeout);
        streams = new String[] { streamName };
    }

    public PartitionedStreamProducer(Set<String> streamNames, Serializer<T> serializer, int connectTimeout, int socketTimeout) {
        super(streamNames, serializer, connectTimeout, socketTimeout);
        streams = streamNames.toArray(new String[0]);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void putRecordsBatch(Collection<T> records) {

        // create the PUT requests and group by partitioned streamName
        Map<String, PutRecordsRequest> splits = mapToStream(records);

        // perform the PUT request on each stream specified
        for (String streamName : splits.keySet()) {

            PutRecordsRequest putRequest = splits.get(streamName);

            List<PutRecordsRequestEntry> puts = putRequest.getRecords();

            PutRecordsResult result = kinesis.putRecords(putRequest);

            // check for errors
            while (result.getFailedRecordCount() > 0) {
                log.log(Level.SEVERE, "Got failed records on stream " + streamName + " count: " + result.toString());
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

    private String chooseStream(T record) {
        String key = serializer.partitionKey(record);

        // if no partition key is defined then no one cares and just do evenly
        if (empty(key))
            return chooseRandomStream();

        return streams[abs(key.hashCode()) % streams.length];
    }

    private String chooseRandomStream() {
        return streams[random.nextInt(streams.length)];
    }

    // Creates PutRecordRequests grouped by the streamName chose via partitionKey
    private Map<String, PutRecordsRequest> mapToStream(Collection<T> records) {

        // partition into streams by key
        final Map<String, List<T>> grouped = records.stream()
                                        .collect(groupingBy(this::chooseStream));

        final Map<String, PutRecordsRequest> result = new HashMap<>(grouped.size());

        for (String streamName : grouped.keySet()) {
            final List<T> recs = grouped.get(streamName);

            // create the initial list of PUT requests
            final List<PutRecordsRequestEntry> puts = new ArrayList<>(records.size());

            for (T rec : recs) {
                puts.add(new PutRecordsRequestEntry()
                        .withPartitionKey(serializer.partitionKey(rec))
                        .withData(serializer.serialize(rec)));
            }

            final PutRecordsRequest putRequest = new PutRecordsRequest()
                    .withRecords(puts)
                    .withStreamName(streamName);

            result.put(streamName, putRequest);
        }

        return result;
    }
}
