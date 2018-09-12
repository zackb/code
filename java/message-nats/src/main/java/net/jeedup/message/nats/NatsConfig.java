package net.jeedup.message.nats;

import net.jeedup.common.net.HostAndPort;
import net.jeedup.message.Serde;

import java.util.Set;

import static net.jeedup.common.collection.CollectionUtil.set;

/**
 * NATS broker nats.io
 * Created by zack on 9/12/18.
 */
public class NatsConfig {

    public Set<HostAndPort> hosts = set(
        new HostAndPort("127.0.0.1", 4222)
    );

    public String subject;

    public Serde<?> serde;

    public int maxReconnects = 60;
}
