package net.jeedup.kafka.serde;

import net.jeedup.message.serde.json.JSONSerde;

import java.util.HashMap;
import java.util.Map;

/**
 * Simple JSON + Kafka serde that translates a message to a map of string to object.
 *
 * Created by zack on 10/3/18.
 */
public class JSONKafkaSerde extends KafkaSerde<Map<String, Object>> {

    @SuppressWarnings("unchecked")
    public JSONKafkaSerde() {
        super(new JSONSerde(HashMap.class));
    }
}
