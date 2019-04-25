package net.jeedup.flink.source.timestamps;

import net.jeedup.common.model.Timestamped;
import org.apache.flink.streaming.api.functions.timestamps.BoundedOutOfOrdernessTimestampExtractor;
import org.apache.flink.streaming.api.windowing.time.Time;

/**
 * Timestamp/Watermark extractor that has a bounded lateness.
 *
 * Created by zack on 3/14/19.
 */
public class JeedupBoundedOutOfOrdernessTimestampExtractor extends BoundedOutOfOrdernessTimestampExtractor<Timestamped> {

    public JeedupBoundedOutOfOrdernessTimestampExtractor(Time maxOutOfOrderness) {
        super(maxOutOfOrderness);
    }

    @Override
    public long extractTimestamp(Timestamped message) {
        return message.timestamp();
    }
}
