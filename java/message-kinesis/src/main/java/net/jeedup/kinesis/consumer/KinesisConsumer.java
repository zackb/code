package net.jeedup.kinesis.consumer;

import com.amazonaws.auth.AWSCredentialsProvider;
import com.amazonaws.services.kinesis.clientlibrary.exceptions.InvalidStateException;
import com.amazonaws.services.kinesis.clientlibrary.exceptions.ShutdownException;
import com.amazonaws.services.kinesis.clientlibrary.exceptions.ThrottlingException;
import com.amazonaws.services.kinesis.clientlibrary.interfaces.IRecordProcessorCheckpointer;
import com.amazonaws.services.kinesis.clientlibrary.interfaces.v2.IRecordProcessor;
import com.amazonaws.services.kinesis.clientlibrary.interfaces.v2.IRecordProcessorFactory;
import com.amazonaws.services.kinesis.clientlibrary.lib.worker.InitialPositionInStream;
import com.amazonaws.services.kinesis.clientlibrary.lib.worker.KinesisClientLibConfiguration;
import com.amazonaws.services.kinesis.clientlibrary.lib.worker.ShutdownReason;
import com.amazonaws.services.kinesis.clientlibrary.lib.worker.Worker;
import com.amazonaws.services.kinesis.clientlibrary.types.InitializationInput;
import com.amazonaws.services.kinesis.clientlibrary.types.ProcessRecordsInput;
import com.amazonaws.services.kinesis.clientlibrary.types.ShutdownInput;
import com.amazonaws.services.kinesis.model.Record;
import com.amazonaws.services.kinesis.metrics.interfaces.MetricsLevel;
import net.jeedup.message.Serde;
import net.jeedup.message.broker.MessageConsumer;
import net.jeedup.message.broker.impl.BaseMessage;

import java.nio.charset.CharacterCodingException;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

import static net.jeedup.kinesis.util.KinesisUtil.createStaticCredentials;
import static net.jeedup.message.broker.BrokerUtil.createClientId;

/**
 * A Kinesis Stream consumer wrapper
 *
 * Created by zack on 9/15/18.
 */
public class KinesisConsumer<T> {

    private static final Logger log = Logger.getLogger(KinesisConsumer.class.getSimpleName());

    private MessageConsumer<T> consumer;

    private KinesisConsumerConfig config;

    private final Serde.Deserializer<T> deserializer;

    private Worker worker;
    private Thread workerThread;


    // number of millis to wait to backoff of an error
    private static final long BACKOFF_TIME_IN_MILLIS = 3000L;

    // number of retries to attempt when an error occurs
    private static final int NUM_RETRIES = 10;

    // Checkpoint about once a minute
    private static final long CHECKPOINT_INTERVAL_MILLIS = 60000L;
    private long nextCheckpointTimeInMillis;

    @SuppressWarnings("unchecked")
    public KinesisConsumer(KinesisConsumerConfig config) {
        this.config = config;
        this.deserializer = (Serde.Deserializer<T>) config.deserializer;
    }

    public void shutdown() {
        if (worker != null) {
            worker.shutdown();
        }
        if (workerThread != null) {
            try {
                workerThread.join();
            } catch (InterruptedException e) {
                log.log(Level.SEVERE, "Failed stopping worker thread", e);
            }
        }
    }

    public void consume(MessageConsumer<T> consumer) {

        // TODO: this wont work for multiple "consumers", MessageBroker interface kinds breaks down here?
        this.consumer = consumer;

        String applicationName = makeApplicationName(config.applicationName);

        // TODO: defaults (env, ~/.aws/config)
        AWSCredentialsProvider credentialsProvider = createStaticCredentials(config.accessKey, config.secretKey);

        String workerId = createClientId();

        KinesisClientLibConfiguration kinesisClientLibConfiguration =
                new KinesisClientLibConfiguration(applicationName, config.streamName, credentialsProvider, workerId);

        kinesisClientLibConfiguration
                .withMetricsLevel(MetricsLevel.NONE)
                .withInitialPositionInStream(InitialPositionInStream.LATEST)
                .withRegionName(config.region);

        IRecordProcessorFactory recordProcessorFactory = RecordProcessor::new;

        worker = new Worker.Builder()
                .recordProcessorFactory(recordProcessorFactory)
                .config(kinesisClientLibConfiguration)
                .build();

        workerThread = new Thread(worker);
        workerThread.start();
    }

    private String makeApplicationName(String pop) {
        return String.format("%s_jeedup_stream_consumer", pop);
    }

    class RecordProcessor implements IRecordProcessor {

        private String shardId;

        // CharsetDecoder is NOT thread safe
        private final CharsetDecoder decoder = Charset.forName("UTF-8").newDecoder();

        @Override
        public void initialize(InitializationInput initializationInput) {
            this.shardId = initializationInput.getShardId();
        }

        @Override
        public void processRecords(ProcessRecordsInput processRecordsInput) {
            IRecordProcessorCheckpointer checkpointer = processRecordsInput.getCheckpointer();
            List<Record> records = processRecordsInput.getRecords();

            // attempt to process all the records with backoff/retries
            processRecordsWithRetries(records);

            // Checkpoint once every checkpoint interval.
            if (System.currentTimeMillis() > nextCheckpointTimeInMillis) {
                checkpoint(checkpointer);
                nextCheckpointTimeInMillis = System.currentTimeMillis() + CHECKPOINT_INTERVAL_MILLIS;
            }
        }

        /**
         * Attempt to process each record with retry/backoff logic to avoid "posion pill" messages
         * @param records to process
         */
        private void processRecordsWithRetries(List<Record> records) {
            List<T> aggregates = new ArrayList<>(records.size());
            for (Record record : records) {
                boolean processed = false;
                for (int i = 0; i < NUM_RETRIES; i++) {
                    try {
                        // TODO: decoding to UTF-8 would not be right for all serializers, move this
                        // to common KinesisSerde stuff
                        String data = decoder.decode(record.getData()).toString();
                        T payload = deserializer.deserialize(data.getBytes()); // WRONG for non-utf8 (json)
                        BaseMessage<T> message = new BaseMessage<>();
                        message.setPayload(payload);
                        // no acks here with checkpointing
                        message.ack = () -> {};
                        consumer.consume(message);
                        processed = true;
                        break;
                    } catch (CharacterCodingException e) {
                        log.log(Level.SEVERE, "Character fooew: ", e);
                    } catch (Throwable t) {
                        log.log(Level.SEVERE, "Unknown exception handling record, continuing", t);
                    }
                    backoff();
                }
                if (!processed) {
                    log.severe("could not process record " + record + ", continuing");
                }
            }
        }

        /**
         * Attempt to checkpoint our shard with retries and backoff
         * @param checkpointer to use to checkpoint
         */
        private void checkpoint(IRecordProcessorCheckpointer checkpointer) {
            log.info("Checkpointing shard " + shardId);

            for (int i = 0; i < NUM_RETRIES - 1; i++) {
                try {
                    checkpointer.checkpoint();
                    break;
                } catch (ShutdownException se) {
                    log.info("Caught shutdown exception, skipping checkpoint.");
                    break;
                } catch (InvalidStateException e) {
                    log.log(Level.SEVERE, "Cannot save checkpoint to the DynamoDB table", e);
                    break;
                } catch (ThrottlingException e) {
                    // Backoff and re-attempt checkpoint upon transient failures
                    if (i >= NUM_RETRIES) {
                        log.log(Level.SEVERE, "Checkpoint failed after " + i + " attepts", e);
                        break;
                    } else {
                        log.log(Level.WARNING, "Transient issue when checkpointing - attempt " + i + " of " + NUM_RETRIES, e);
                    }
                }
                backoff();
            }
        }

        private void backoff() {
            try {
                Thread.sleep(BACKOFF_TIME_IN_MILLIS);
            } catch (InterruptedException e) {
                log.log(Level.WARNING, "Interrupted sleep", e);
            }
        }

        @Override
        public void shutdown(ShutdownInput shutdownInput) {
            ShutdownReason reason = shutdownInput.getShutdownReason();
            IRecordProcessorCheckpointer checkpointer = shutdownInput.getCheckpointer();
            log.info("Shutdown kinesis record processor: " + reason.name());

            // important to checkpoint after reaching end of shard, so we can start processing data from child shards.
            if (reason == ShutdownReason.TERMINATE) {
                checkpoint(checkpointer);
            }
        }
    }
}
