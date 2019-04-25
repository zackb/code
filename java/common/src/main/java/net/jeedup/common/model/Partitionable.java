package net.jeedup.common.model;

/**
 * Interface to declare partition keys of records.
 * Created by zack on 2/11/19.
 */
public interface Partitionable {
    /**
     * Get a partition key for this record.
     * @return key to partition data by
     */
    String partitionKey();
}
