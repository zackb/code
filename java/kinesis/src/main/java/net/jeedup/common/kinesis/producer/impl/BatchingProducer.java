package net.jeedup.common.kinesis.producer.impl;

import net.jeedup.common.kinesis.producer.KinesisProducer;
import net.jeedup.common.collection.CollectionUtil;

import java.util.ArrayList;
import java.util.Collection;

import static net.jeedup.common.util.Util.empty;

/**
 * Convenience class to handle kinesis batch size limits. Subclasses should implement {@link #putRecordsBatch(Collection)}
 * to be sure they get a non-empty size limited batch to process.
 *
 * Created by zack on 4/17/18.
 */
public abstract class BatchingProducer<T> implements KinesisProducer<T> {

    private static final int MAX_BATCH_SIZE = 500;

    @Override
    public void putRecords(Collection<T> records) {
        // check for empty request
        if (empty(records)) return;

        // kinesis limit
        if (records.size() > MAX_BATCH_SIZE) {
            // split into 500 chunk batches
            CollectionUtil.batches(new ArrayList<>(records), MAX_BATCH_SIZE)
                    .forEach(this::putRecordsBatch);
        } else {
            putRecordsBatch(records);
        }
    }

    /**
     * Abstract routine to perform the actual write to Kinesis. Subclasses can be guaranteed records is a
     * non-empty collection that respects Kinesis limits
     * @param records to write to Kinesis
     */
    protected abstract void putRecordsBatch(Collection<T> records);
}
