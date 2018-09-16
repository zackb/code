package net.jeedup.aws.lambda.kinesis.deaggregate;

import com.amazonaws.services.kinesis.clientlibrary.types.UserRecord;
import com.amazonaws.services.kinesis.model.Record;
import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.RequestHandler;
import com.amazonaws.services.lambda.runtime.events.KinesisAnalyticsInputPreprocessingResponse;
import com.amazonaws.services.lambda.runtime.events.KinesisAnalyticsStreamsInputPreprocessingEvent;
import net.jeedup.aws.lambda.kinesis.LambdaKinesisUtil;
import net.jeedup.common.json.JSON;

import java.nio.ByteBuffer;
import java.nio.charset.CharacterCodingException;
import java.util.*;

/**
 * Simple lambda which accepts records aggregated by the KPL and returns deaggregated records.
 * This is used for Kinesis Analytics record transformation function as Analytics does not support
 * the KPL aggreagation format??
 * https://forums.aws.amazon.com/message.jspa?messageID=736716
 *
 * Created by zack on 9/15/18.
 */
public class KinesisDeaggregatorLambda implements
        RequestHandler<KinesisAnalyticsStreamsInputPreprocessingEvent, KinesisAnalyticsInputPreprocessingResponse> {

    @Override
    public KinesisAnalyticsInputPreprocessingResponse handleRequest(KinesisAnalyticsStreamsInputPreprocessingEvent event, Context context) {
        List<KinesisAnalyticsInputPreprocessingResponse.Record> records = new ArrayList<>();
        KinesisAnalyticsInputPreprocessingResponse response = new KinesisAnalyticsInputPreprocessingResponse(records);

        for (KinesisAnalyticsStreamsInputPreprocessingEvent.Record analyticsRecord : event.getRecords()) {
            Record kinesisRecord = new Record()
                    .withData(analyticsRecord.getData())
                    .withApproximateArrivalTimestamp(new Date(analyticsRecord.kinesisStreamRecordMetadata.approximateArrivalTimestamp))
                    .withPartitionKey(analyticsRecord.kinesisStreamRecordMetadata.partitionKey)
                    .withSequenceNumber(analyticsRecord.kinesisStreamRecordMetadata.sequenceNumber);

            List<UserRecord> userRecords = UserRecord.deaggregate(Arrays.asList(kinesisRecord));
            List<Map<String, Object>> jsonRecords = new ArrayList<>(userRecords.size());
            for (UserRecord userRecord : userRecords) {
                try {
                    String data = LambdaKinesisUtil.decoder.decode(userRecord.getData()).toString();
                    Map<String, Object> json =  JSON.decode(data);
                    jsonRecords.add(json);
                } catch (CharacterCodingException e) {
                    e.printStackTrace();
                }

            }
            ByteBuffer bytes = ByteBuffer.wrap(JSON.encodeBytes(jsonRecords));
            records.add(
                    new KinesisAnalyticsInputPreprocessingResponse.Record(analyticsRecord.recordId,
                            KinesisAnalyticsInputPreprocessingResponse.Result.Ok, bytes));
        }

        return response;
    }
}
