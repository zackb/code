package net.jeedup.message.nats.streaming;

import net.jeedup.message.Serde;

import java.util.Set;

import static net.jeedup.common.collection.CollectionUtil.set;

/**
 * Config values for NATS Streaming Server client
 *
 * Created by zack on 9/11/18.
 */
public class NatsStreamingConfig {

    public String clusterId    = "jeedup_cluster";
    public String queueName    = "jeedup_group";
    public String queueGroup   = "jeedup.workers";
    public String host;
    public int port;

    public boolean manualAcks = true;

    public Set<String> queueNames = set("jeedup1");

    public Serde<?> serde;

    // the maximum number of unacknowledged messages that the publisher may have at one time
    public int maxInFlight     = 1024;

    // number of seconds to wait for an ACK before a message is considered failed and should be redelivered
    public int ackWaitSecs     = 60;

    public String getNatsUrl() {
        return String.format("nats://%s:%d", host, port);
    }
}
