package net.jeedup.flink.source.timestamps;

import net.jeedup.common.model.Timestamped;
import org.apache.flink.streaming.api.functions.AssignerWithPeriodicWatermarks;
import org.apache.flink.streaming.api.watermark.Watermark;

/**
 * This generator generates watermarks that are lagging behind processing time by a fixed amount.
 * It assumes that elements arrive in Flink after a bounded delay.
 *
 * Created by zack on 3/13/19.
 */
public class JeedupTimeLagWatermarkGenerator implements AssignerWithPeriodicWatermarks<Timestamped> {

    private final long maxTimeLag = 5000; // 5 seconds

    @Override
    public long extractTimestamp(Timestamped message, long previousElementTimestamp) {
        return message.timestamp();
    }

    @Override
    public Watermark getCurrentWatermark() {
        // return the watermark as current time minus the maximum time lag
        return new Watermark(System.currentTimeMillis() - maxTimeLag);
    }
}
