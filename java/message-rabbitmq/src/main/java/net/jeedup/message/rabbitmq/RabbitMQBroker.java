package net.jeedup.message.rabbitmq;

import com.rabbitmq.client.*;
import com.rabbitmq.client.impl.DefaultExceptionHandler;
import net.jeedup.message.Message;
import net.jeedup.message.Serde;
import net.jeedup.message.broker.MessageBroker;
import net.jeedup.message.broker.MessageConsumer;
import net.jeedup.message.broker.impl.BaseMessage;

import java.io.IOException;
import java.util.Collection;
import java.util.Date;
import java.util.logging.Level;
import java.util.logging.Logger;

import static net.jeedup.common.util.Util.empty;


/**
 * RabbitMQ broker
 * Created by zack on 9/11/18.
 */
public class RabbitMQBroker<T> implements MessageBroker<T> {

    private static final Logger log = Logger.getLogger(RabbitMQBroker.class.getName());

    private final RabbitMQConfig config;

    private Connection connection;

    private ThreadLocal<Channel> channels;

    private final Serde<T> serde;

    @SuppressWarnings("unchecked")
    public RabbitMQBroker(RabbitMQConfig config) {
        this.config = config;
        this.serde = (Serde<T>) config.serde;
    }

    @Override
    public void publish(Message<T> message) throws Exception {
        channels.get().basicPublish(config.queueName, config.queueName, null, serde.serialize(message.getPayload()));
    }

    @Override
    public void publish(Collection<Message<T>> messages) throws Exception {
        for (Message<T> message : messages) {
            publish(message);
        }
    }

    @Override
    public void consume(MessageConsumer<T> consumer) throws Exception {
        Channel channel = channels.get();
        Consumer c = new DefaultConsumer(channel) {
            @Override
            public void handleDelivery(String consumerTag, Envelope envelope,
                                       AMQP.BasicProperties properties, byte[] body) throws IOException {
                try {
                    T payload= serde.deserialize(body);
                    BaseMessage<T> message = new BaseMessage<>();
                    message.setPayload(payload);
                    Date ts = properties.getTimestamp();
                    if (ts != null)
                        message.setTimestamp(ts.getTime());
                    message.ack = () -> ack(channel, envelope);
                    consumer.consume(message);
                } catch (Throwable e) {
                    log.log(Level.SEVERE, "Failed consuming message", e);
                    nack(channel, envelope);
                }
                //log.finest("Received message: " + body.length);
            }
        };

        channel.basicConsume(config.queueName, c);
    }

    private void ack(Channel channel, Envelope envelope) {
        try {
            channel.basicAck(envelope.getDeliveryTag(), false);
        } catch (IOException e) {
            log.log(Level.SEVERE, "Failed acknowledging message", e);
        }
    }

    private void nack(Channel channel, Envelope envelope) {
        try {
            channel.basicNack(envelope.getDeliveryTag(), false, config.requeueOnError);
        } catch (IOException e) {
            log.log(Level.SEVERE, "Failed negatively acknowledging message", e);
        }
    }

    @Override
    public void startup() throws Exception {
        ConnectionFactory factory = new ConnectionFactory();
        factory.setHost(config.host);
        factory.setPort(config.port);
        if (!empty(config.username)) {
            factory.setUsername(config.username);
            factory.setPassword(config.password);
        }
        factory.setExceptionHandler(new DefaultExceptionHandler());

        connection = factory.newConnection();

        channels = ThreadLocal.withInitial(() -> {
            Channel channel = null;
            try {
                channel = connection.createChannel();
                channel.exchangeDeclare(config.queueName, "direct", config.durable);
                channel.queueDeclare(config.queueName, config.durable, false, config.autoDelete, null);
                channel.queueBind(config.queueName, config.queueName, config.queueName); // I know...
                channel.basicQos(config.prefetch);
            } catch (IOException e) {
                log.log(Level.SEVERE, "Failed creating RabbitMQ channel", e);
            }
            return channel;
        });

    }

    @Override
    public void shutdown() throws Exception {
        if (connection != null)
            connection.close();
        connection = null;
    }
}
