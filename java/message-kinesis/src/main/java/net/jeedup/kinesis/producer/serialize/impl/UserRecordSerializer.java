package net.jeedup.kinesis.producer.serialize.impl;

import com.amazonaws.services.kinesis.clientlibrary.types.UserRecord;
import net.jeedup.kinesis.producer.serialize.Serializer;

import java.nio.ByteBuffer;

/**
 * Helper implementation for generic UserRecords.
 * Created by zack on 4/17/18.
 */
public class UserRecordSerializer implements Serializer<UserRecord> {

    @Override
    public ByteBuffer serialize(UserRecord object) {
        return object.getData();
    }

    @Override
    public UserRecord deserialize(ByteBuffer bytes) {
        throw new UnsupportedOperationException("Can not generically deserialize a UserRecord");
    }

    @Override
    public String partitionKey(UserRecord userRecord) {
        return userRecord.getPartitionKey();
    }
}
