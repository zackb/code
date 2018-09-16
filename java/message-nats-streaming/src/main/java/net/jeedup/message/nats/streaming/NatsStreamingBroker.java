package net.jeedup.message.nats.streaming;

import io.nats.client.Connection;
import io.nats.client.Consumer;
import io.nats.client.ErrorListener;
import io.nats.client.Nats;
import io.nats.streaming.NatsStreaming;
import io.nats.streaming.Options;
import io.nats.streaming.StreamingConnection;
import io.nats.streaming.SubscriptionOptions;
import net.jeedup.message.Message;
import net.jeedup.message.Serde;
import net.jeedup.message.broker.MessageBroker;
import net.jeedup.message.broker.MessageConsumer;
import net.jeedup.message.broker.impl.BaseMessage;

import java.io.IOException;
import java.util.Collection;
import java.util.Set;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;

import static net.jeedup.common.collection.CollectionUtil.set;
import static net.jeedup.common.util.Util.empty;
import static net.jeedup.message.broker.BrokerUtil.createClientId;

/**
 * Created by zack on 9/11/18.
 */
public class NatsStreamingBroker<T> implements MessageBroker<T> {

    private static final Logger log = Logger.getLogger(NatsStreamingBroker.class.getName());

    private final NatsStreamingConfig config;

    private final Serde<T> serde;

    private Connection natsConnection;
    private StreamingConnection connection;

    @SuppressWarnings("unchecked")
    public NatsStreamingBroker(NatsStreamingConfig config) {
        this.config = config;
        this.serde = (Serde<T>) config.serde;
    }

    @Override
    public void publish(Message<T> message) throws Exception {
        connection.publish(config.subject, serde.serialize(message.getPayload()), (nuid, ex) -> {
            if (ex != null) log.log(Level.SEVERE, "Failed publishing", ex);
        });
    }

    @Override
    public void publish(Collection<Message<T>> messages) throws Exception {
        for (Message<T> message : messages) {
            publish(message);
        }
    }

    @Override
    public void consume(MessageConsumer<T> consumer) throws Exception {

        SubscriptionOptions.Builder builder =
            new SubscriptionOptions.Builder()
                .durableName("durable") // name here doesn't matter as long as its non-empty and the same on all consumers
                .ackWait(config.ackWaitSecs, TimeUnit.SECONDS)
                .maxInFlight(config.maxInFlight); // library default is 1024

        if (config.manualAcks)
            builder.manualAcks();

        SubscriptionOptions subscriptionOptions = builder.build();

        Set<String> queueNames = config.subjects;

        if (empty(queueNames)) {
            queueNames = set(config.subject);
        }

        for (String queueName : queueNames) {
            connection.subscribe(queueName, config.queueGroup, m -> {
                String payload = null;

                if (m == null) {
                    log.severe("WHA?!? Message is null?");
                    return;
                }

                if (m.isRedelivered()) {
                    log.warning("Got redelivered message: " + new String(m.getData()));
                }

                try {
                    T data = serde.deserialize(m.getData());
                    BaseMessage<T> message = new BaseMessage<>();
                    message.setPayload(data);
                    message.setTimestamp(m.getTimestamp());
                    message.ack = () -> ack(m);
                    consumer.consume(message);
                } catch (Throwable t) {
                    // the consumer threw an error handling the message, need to handle this here
                    // to protect from SILENT nats failure. This will stop consuming messages if trickled up
                    log.log(Level.SEVERE, "Failed consuming message: " + payload, t);
                }
            }, subscriptionOptions);
        }
    }

    @Override
    public void startup() throws IOException, InterruptedException {
        io.nats.client.Options options = new io.nats.client.Options.Builder()
                .server(config.getNatsUrl())
                .maxReconnects(config.maxReconnects)
                .connectionListener((conn, type) -> {
                    switch (type) {
                        case CONNECTED:
                            log.info("NATS connected: " + type);
                            break;
                        case CLOSED:
                            log.warning("NATS connection closed: " + type);
                            break;
                        case DISCONNECTED:
                            log.warning("NATS disconncted: " + type);
                            break;
                        case RECONNECTED:
                            log.warning("NATS reconnected, attempting reconnect: " + type);
                            restart();
                            break;
                        case RESUBSCRIBED:
                            log.warning("NATS resubscribed: " + type);
                            break;
                        case DISCOVERED_SERVERS:
                            log.warning("NATS discovered servers: " + type);
                            break;
                        default:
                            log.severe("NATS Unknown Event: " + type);
                            break;
                    }
                })
                .errorListener(new ErrorListener() {
                    @Override
                    public void errorOccurred(Connection conn, String error) {
                        log.log(Level.SEVERE, "NATS exception, error occurred: " + error);
                    }

                    @Override
                    public void exceptionOccurred(Connection conn, Exception ex) {
                        log.log(Level.SEVERE, "NATS exception, exception occurred: ", ex);
                    }

                    @Override
                    public void slowConsumerDetected(Connection conn, Consumer consumer) {
                        log.log(Level.SEVERE, "NATS Slow Consumer Detected: " + consumer.getPendingMessageCount());
                    }
                })
                .build();

        natsConnection = Nats.connect(options);

        String clientId = createClientId();

        log.config("Using NATS Streaming clientId: " + clientId);

        Options streamingOptions = new Options.Builder()
                .natsConn(natsConnection)
                .build();

        connection = NatsStreaming.connect(config.clusterId, clientId, streamingOptions);
    }

    @Override
    public void shutdown() throws Exception {
        if (connection != null) {
            connection.close();
            connection = null;
        }

        if (natsConnection != null) {
            natsConnection.close();
            natsConnection = null;
        }
    }

    private void restart() {
        try {
            shutdown();
        } catch (Throwable e) {
            log.log(Level.SEVERE, "Reconnect: Failed stopping queue", e);
        }
        try {
            startup();
        } catch (Throwable e) {
            log.log(Level.SEVERE, "Reconnect: Failed restarting queue", e);
        }
    }

    private void ack(io.nats.streaming.Message m) {
        try {
            m.ack();
        } catch (NullPointerException e) {
            log.log(Level.WARNING, "Tried to ack on a null connection", e);
        } catch (Throwable e) {
            log.log(Level.SEVERE, "Failed acknowledging message", e);
        }
    }
}
