package net.jeedup.aws.lambda.kinesis.fanout;

import com.amazonaws.services.kinesis.clientlibrary.types.UserRecord;
import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.RequestHandler;
import com.amazonaws.services.lambda.runtime.events.KinesisEvent;
import net.jeedup.kinesis.producer.KinesisProducer;
import net.jeedup.kinesis.producer.impl.StreamProducer;
import net.jeedup.kinesis.producer.serialize.impl.UserRecordSerializer;

import java.util.List;

import static net.jeedup.aws.lambda.kinesis.LambdaKinesisUtil.deaggregate;


/**
 * Fanout a single Kinesis Stream into muliple streams. Reads from the Kinesis Stream input and writes to
 * all Kinesis Streams specified in the environment variable TARGET_STREAMS (comma separated)
 *
 * Created by zack on 9/15/18.
 */
public class KinesisFanoutLambda implements RequestHandler<KinesisEvent, Void> {

    private final KinesisProducer<UserRecord> kinesisProducer;
    private final Config config = new Config();

    public KinesisFanoutLambda() {
        kinesisProducer = new StreamProducer<>(config.targetStreamNames, new UserRecordSerializer());
    }

    @Override
    public Void handleRequest(KinesisEvent input, Context context) {
        // deaggregate the batch of incoming records into single records
        List<UserRecord> records = deaggregate(input.getRecords());
        kinesisProducer.putRecords(records);
        return null;
    }
}
