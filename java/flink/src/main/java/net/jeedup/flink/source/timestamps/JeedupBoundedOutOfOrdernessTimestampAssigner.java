package net.jeedup.flink.source.timestamps;

import net.jeedup.common.model.Timestamped;
import org.apache.flink.streaming.api.functions.timestamps.BoundedOutOfOrdernessTimestampExtractor;
import org.apache.flink.streaming.api.windowing.time.Time;

/**
 * Timestamp/Watermark extractor that has a bounded lateness.
 *
 * Created by zack on 3/19/19.
 */
public class JeedupBoundedOutOfOrdernessTimestampAssigner<T> extends BoundedOutOfOrdernessTimestampExtractor<T> {

    public JeedupBoundedOutOfOrdernessTimestampAssigner(Time maxOutOfOrderness) {
        super(maxOutOfOrderness);
    }

    @Override
    public long extractTimestamp(T element) {
        long ts = 0;
        if (element instanceof Timestamped) {
            ts = ((Timestamped) element).timestamp();
        }
        return ts;
    }
}
