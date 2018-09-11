package net.jeedup.common.util;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.TimeZone;

/**
 * Utilities for working with dates, time, and timestamps
 *
 * Created by zack on 3/6/18.
 */
public class DateTimeUtil {

    private static final DateFormat dateFormatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
    private static final DateFormat microsecondDateFormatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSSSSS");

    static {
        dateFormatter.setTimeZone(TimeZone.getTimeZone("GMT"));
        microsecondDateFormatter.setTimeZone(TimeZone.getTimeZone("GMT"));
    }

    /**
     * Format a timestamp using GMT in ISO-8601 format yyyy-MM-dd HH:mm:ss
     * @param timestamp milliseconds since the unix epoch
     * @return ISO-8601 formatted timestamp string with GMT
     */
    public static String formatTimestamp(long timestamp) {
        return dateFormatter.format(new Date(timestamp));
    }

    /**
     * Format a timestamp using GMT in ISO-8601 format with microsecond precision yyyy-MM-dd HH:mm:ss.SSSSSS
     * @param timestamp milliseconds since the unix epoch
     * @return ISO-8601 formatted timestamp string with GMT and microsecond precision
     */
    public static String formatTimestampMicros(long timestamp) {
        return microsecondDateFormatter.format(new Date(timestamp));
    }

    /**
     * Format the current time using GMT in ISO-8601 format with microsecond precision yyyy-MM-dd HH:mm:ss.SSSSSS
     * @return ISO-8601 formatted timestamp string with GMT and microsecond precision
     */
    public static String currentTimestampMicros() {
        return microsecondDateFormatter.format(new Date());
    }
}
