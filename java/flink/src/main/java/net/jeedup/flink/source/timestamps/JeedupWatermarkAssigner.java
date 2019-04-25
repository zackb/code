package net.jeedup.flink.source.timestamps;

import javax.annotation.Nullable;

import net.jeedup.common.model.Timestamped;
import org.apache.flink.streaming.api.functions.AssignerWithPeriodicWatermarks;
import org.apache.flink.streaming.api.watermark.Watermark;

/**
 * FOR TESTING ONLY not production worthy. Watermark assigner for Kinesis as Kinesis does not have native timestamps.
 * Created by zack on 3/11/19.
 */
public class JeedupWatermarkAssigner<T extends Timestamped> implements AssignerWithPeriodicWatermarks<T> {

    private long prevTs = 0;

    @Nullable
    @Override
    public Watermark getCurrentWatermark() {
        if (prevTs > 0)
            return new Watermark(prevTs);
        return null;
    }

    @Override
    public long extractTimestamp(T element, long previousElementTimestamp) {
        long ts = element.timestamp();

        if (ts == 0)
            ts = previousElementTimestamp;

        if (ts > 0)
            prevTs = ts;

        return ts;
    }
}
