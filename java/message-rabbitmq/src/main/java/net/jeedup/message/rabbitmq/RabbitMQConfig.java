package net.jeedup.message.rabbitmq;

import net.jeedup.message.Serde;

/**
 * RabbitMQ Server config
 * Created by zack on 9/11/18.
 */
public class RabbitMQConfig {
    public String queueName;

    public String host;
    public int port = 5672;

    public String username;
    public String password;
    public boolean durable = true;
    public boolean autoDelete = false;
    public int prefetch = 1;
    public boolean requeueOnError = true;

    public Serde<?> serde;
}
