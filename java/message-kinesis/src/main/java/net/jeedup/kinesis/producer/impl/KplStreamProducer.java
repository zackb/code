package net.jeedup.kinesis.producer.impl;

import com.amazonaws.auth.AWSCredentialsProvider;
import com.amazonaws.auth.AWSStaticCredentialsProvider;
import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.services.kinesis.producer.KinesisProducerConfiguration;
import com.amazonaws.services.kinesis.producer.UserRecordFailedException;
import com.amazonaws.services.kinesis.producer.UserRecordResult;
import com.google.common.util.concurrent.FutureCallback;
import com.google.common.util.concurrent.Futures;
import com.google.common.util.concurrent.ListenableFuture;
import net.jeedup.common.util.FileUtil;
import net.jeedup.kinesis.producer.KinesisProducer;
import net.jeedup.kinesis.producer.KinesisProducerConfig;
import net.jeedup.kinesis.producer.serialize.Serializer;

import java.util.Collection;
import java.util.logging.Level;
import java.util.logging.Logger;

import static java.util.stream.Collectors.toList;

/**
 * A Kinesis Stream producer that uses the KPL rather than the normal client
 * interface.
 *
 * Created by zack on 9/15/18.
 */
public class KplStreamProducer<T> implements KinesisProducer<T> {

    private static final Logger log = Logger.getLogger(KplStreamProducer.class.getSimpleName());

    private final com.amazonaws.services.kinesis.producer.KinesisProducer kinesis;

    private final Serializer<T> serializer;

    private final KinesisProducerConfig config;

    @SuppressWarnings("unchecked")
    public KplStreamProducer(KinesisProducerConfig config) {

        this.config = config;
        this.serializer = (Serializer<T>) config.serializer;

        AWSCredentialsProvider credentialsProvider = createCredentials();

        KinesisProducerConfiguration kpc = new KinesisProducerConfiguration()
                .setRecordMaxBufferedTime(config.maxBufferedTimeMillis) // library default is 100ms
                .setAggregationMaxCount(config.maxAggregationCount) // library default is 4294967295
                .setMaxConnections(24) // this is the default
                .setRequestTimeout(120000)
                .setCredentialsProvider(credentialsProvider)
                .setRecordTtl(Integer.MAX_VALUE)
                .setMetricsLevel("none")
                .setTempDirectory(FileUtil.tempDir() + "jeedup_message_kinesis")
                .setRegion(config.region);

        kinesis = new com.amazonaws.services.kinesis.producer.KinesisProducer(kpc);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void putRecords(Collection<T> records) {
        records.forEach(this::putRecord);
    }

    @Override
    public void putRecord(T message) {

        while (kinesis.getOutstandingRecordsCount() > 10000) {
            log.warning(config.streamName + " : " + "Outstanding record count: " + kinesis.getOutstandingRecordsCount());
            sleep(100);
        }

        ListenableFuture<UserRecordResult> future =
                kinesis.addUserRecord(config.streamName,
                        serializer.partitionKey(message),
                        serializer.serialize(message));

        Futures.addCallback(future, callback);
    }


    FutureCallback<UserRecordResult> callback = new FutureCallback<UserRecordResult>() {

        @Override
        public void onSuccess(UserRecordResult result) {

            if (!result.isSuccessful()) {
                log.severe("Not successful PUT attempt");
                if (result.getAttempts() != null) {
                    String errors =
                            String.join("\n", result.getAttempts().stream()
                                    .map(a -> String.format(
                                            "Delay after prev attempt: %d ms, "
                                                    + "Duration: %d ms, Code: %s, "
                                                    + "Message: %s",
                                            a.getDelay(), a.getDuration(),
                                            a.getErrorCode(),
                                            a.getErrorMessage()))
                                    .collect(toList()));
                    log.severe(errors);
                }
            }

            long totalTime = result.getAttempts().stream()
                    .mapToLong(a -> a.getDelay() + a.getDuration())
                    .sum();

            if (totalTime > config.maxBufferedTimeMillis * 4) {
                log.warning(config.streamName + " taking longer to write to kinesis than the configured value: " + totalTime);
            }
        }

        @Override
        public void onFailure(Throwable t) {

            log.log(Level.SEVERE, "Failed sending record to kinesis", t);

            String errors = t.getMessage();

            if (t instanceof UserRecordFailedException) {
                UserRecordFailedException e =
                        (UserRecordFailedException) t;
                UserRecordResult result = e.getResult();

                errors =
                        String.join("\n", result.getAttempts().stream()
                                .map(a -> String.format(
                                        "Delay after prev attempt: %d ms, "
                                                + "Duration: %d ms, Code: %s, "
                                                + "Message: %s",
                                        a.getDelay(), a.getDuration(),
                                        a.getErrorCode(),
                                        a.getErrorMessage()))
                                .collect(toList()));

            }

            log.severe(errors);
        }
    };

    private void sleep(long duration) {
        try {
            Thread.sleep(duration);
        } catch (InterruptedException e) {
            log.log(Level.SEVERE, "Sleep interrupted", e);
        }
    }

    @Override
    public void startup() throws Exception { }

    /**
     * {@inheritDoc}
     */
    @Override
    public void shutdown() {
        if (kinesis != null) {
            kinesis.flushSync();
            kinesis.destroy();
        }
    }

    /**
     * // TODO: Default credentials
     * Create credentials to be used with AWS SDK calls
     * @return basic credentials using access key and secret key
     */
    private AWSCredentialsProvider createCredentials() {
        return new AWSStaticCredentialsProvider(new BasicAWSCredentials(config.accessKey, config.secretKey));
    }
}
