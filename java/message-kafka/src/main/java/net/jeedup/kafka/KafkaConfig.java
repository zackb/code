package net.jeedup.kafka;

import net.jeedup.common.net.HostAndPort;
import net.jeedup.kafka.serde.JavaKafkaSerde;

import java.util.Set;

import static java.util.stream.Collectors.joining;
import static net.jeedup.common.collection.CollectionUtil.set;

/**
 * Kafka consumer and producer config
 *
 * Created by zack on 10/2/18.
 */
public class KafkaConfig {

    public Set<HostAndPort> hosts = set(
            new HostAndPort("127.0.0.1", 9092)
    );

    public String topic;

    public String groupName = "jeedup.workers";

    // public String serdeClassName = JSONKafkaSerde.class.getName();
    public String serdeClassName = JavaKafkaSerde.class.getName();


    // producer only config

    public boolean exactlyOnce = true;

    public String acks = "1"; // or "all";

    public int retries = 1;

    /**
     * Returns a string usable in the "bootstrap.servers" kafka config property
     * @return host:port,host:port string
     */
    public String bootstrapServerString() {
        return hosts.stream()
                .map(hp -> hp.host + ":" + hp.port)
                .collect(joining(","));
    }
}
