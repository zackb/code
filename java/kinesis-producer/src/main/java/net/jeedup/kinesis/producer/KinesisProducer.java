package net.jeedup.kinesis.producer;

import java.util.Collection;

/**
 * Utility to wrap the Kinesis client's producer functionality to do common things like:
 *  - Marshall records
 *  - Deaggregate records
 *  - Handle Kinesis limits (500 records per PUT, 5MB per PUT)
 *  - Handle errors and retries
 *
 * Note: This is NOT the KPL and should only be used if the KPL is not an option
 *
 * Created by zack on 4/17/18.
 */
public interface KinesisProducer<T> {

    /**
     * Write records to Kinesis
     * @param records to write
     */
    void putRecords(Collection<T> records);
}
