package net.jeedup.aws.lambda.kinesis;

import com.amazonaws.services.kinesis.clientlibrary.types.UserRecord;
import com.amazonaws.services.lambda.runtime.events.KinesisEvent;

import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;

/**
 * Utility methods for working with Kinesis Streams
 * Created by zack on 4/16/18.
 */
public class LambdaKinesisUtil {

    public static final String CHARSET = "UTF-8";
    public static final CharsetDecoder decoder = Charset.forName(CHARSET).newDecoder();

    /**
     * Method to bulk deaggregate a set of Kinesis user records from a list of
     * Kinesis Event Records.
     *
     * @param inputRecords
     *            The Kinesis Event Records provided by AWS Lambda
     * @return A list of Kinesis UserRecord objects obtained by deaggregating
     *         the input list of KinesisEventRecords
     */
    public static List<UserRecord> deaggregate(List<KinesisEvent.KinesisEventRecord> inputRecords) {
        List<UserRecord> outputRecords = new LinkedList<>();
        for (KinesisEvent.KinesisEventRecord inputRecord : inputRecords) {
            outputRecords.addAll(deaggregate(inputRecord));
        }
        return outputRecords;
    }

    /**
     * Method to deaggregate a single Kinesis record into one or more
     * Kinesis user records.
     *
     * @param inputRecord
     *            The single KinesisEventRecord to deaggregate
     * @return A list of Kinesis UserRecord objects obtained by deaggregating
     *         the input KinesisEventRecord
     */
    public static List<UserRecord> deaggregate(KinesisEvent.KinesisEventRecord inputRecord) {
        return UserRecord.deaggregate(Arrays.asList(inputRecord.getKinesis()));
    }
}
