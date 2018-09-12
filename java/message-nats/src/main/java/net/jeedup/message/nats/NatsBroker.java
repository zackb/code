package net.jeedup.message.nats;

import io.nats.client.*;
import net.jeedup.common.net.HostAndPort;
import net.jeedup.message.Message;
import net.jeedup.message.Serde;
import net.jeedup.message.broker.MessageBroker;
import net.jeedup.message.broker.MessageConsumer;
import net.jeedup.message.broker.impl.BaseMessage;

import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * NATS broker nats.io
 * Created by zack on 9/12/18.
 */
public class NatsBroker<T> implements MessageBroker<T> {

    private static final Logger log = Logger.getLogger(NatsBroker.class.getName());

    private Connection connection;
    private final NatsConfig config;

    private final Serde<T> serde;

    private boolean closing = false;

    @SuppressWarnings("unchecked")
    public NatsBroker(NatsConfig config) {
        this.config = config;
        this.serde = (Serde<T>) config.serde;
    }

    @Override
    public void publish(Message<T> message) throws Exception {
        connection.publish(config.subject, serde.serialize(message.getPayload()));
    }

    @Override
    public void consume(MessageConsumer<T> consumer) throws Exception {
        Dispatcher dispatcher = connection.createDispatcher((msg) -> {
            T data = serde.deserialize(msg.getData());
            BaseMessage<T> message = new BaseMessage<>();
            message.setPayload(data);
            // no acking on nats
            message.ack = () -> {};
            consumer.consume(message);
            // log.finest("Received messsage: " + bytes.length);
        });
        dispatcher.subscribe(config.subject);
    }

    @Override
    public void startup() throws Exception {
        Options.Builder builder = new Options.Builder()
                .maxReconnects(config.maxReconnects)
                .connectionListener((conn, type) ->
                        log.info("NATS Connection Event: " + type.toString()))
                .errorListener(new ErrorListener() {
                    @Override
                    public void errorOccurred(Connection conn, String error) {
                        log.severe("NATS Error occurred: " + error);
                    }

                    @Override
                    public void exceptionOccurred(Connection conn, Exception exp) {
                        if (!closing) {
                            log.log(Level.SEVERE, "NATS Exception occurred", exp);
                        }
                    }

                    @Override
                    public void slowConsumerDetected(Connection conn, Consumer consumer) {
                        log.log(Level.SEVERE, "NATS Slow consumer detected!");
                    }
                });


        for (HostAndPort hp : config.hosts) {
            builder.server(String.format("nats://%s:%d", hp.host, hp.port));
        }

        Options options = builder.build();

        connection = Nats.connect(options);
    }

    @Override
    public void shutdown() throws Exception {
        closing = true;
        if (connection != null) {
            connection.close();
            connection = null;
        }
    }
}
