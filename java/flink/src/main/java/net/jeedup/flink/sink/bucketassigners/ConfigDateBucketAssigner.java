package net.jeedup.flink.sink.bucketassigners;

import net.jeedup.common.model.Partitionable;
import net.jeedup.common.model.Timestamped;
import org.apache.flink.core.io.SimpleVersionedSerializer;
import org.apache.flink.streaming.api.functions.sink.filesystem.BucketAssigner;
import org.apache.flink.streaming.api.functions.sink.filesystem.bucketassigners.SimpleVersionedStringSerializer;

import java.time.Instant;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;

/**
 * StreamingFileSink bucket assigner which uses partition keys and the date for hadoop style partitions.
 *
 * Created by zack on 2/11/19.
 */
public class ConfigDateBucketAssigner<T> implements BucketAssigner<T, String> {

    private static final String formatString = "%s/part=%s";

    private static final String dateFormatString = "'year='yyyy'/month='MM'/day='dd'/hour='HH";

    private transient DateTimeFormatter dateTimeFormatter;

    @Override
    public String getBucketId(T element, Context context) {
        if (dateTimeFormatter == null) {
            dateTimeFormatter = DateTimeFormatter.ofPattern(dateFormatString).withZone(ZoneId.of("UTC"));
        }

        long epoch = context.currentProcessingTime();
        String part = "unknown";

        if (element instanceof Timestamped) {
            epoch = ((Timestamped)element).timestamp();
        }

        if (element instanceof Partitionable) {
            part = ((Partitionable)element).partitionKey();
        } else {
            throw new RuntimeException("I dont know how to partition this: " + element.getClass());
        }

        String dateString = dateTimeFormatter.format(Instant.ofEpochMilli(epoch));

        return String.format(formatString, dateString, part);
    }

    @Override
    public SimpleVersionedSerializer<String> getSerializer() {
        return SimpleVersionedStringSerializer.INSTANCE;
    }
}
