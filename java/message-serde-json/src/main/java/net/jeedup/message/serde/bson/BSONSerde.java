package net.jeedup.message.serde.bson;

import net.jeedup.message.impl.BaseSerde;

/**
 * TODO
 * Created by zack on 9/11/18.
 */
public class BSONSerde<T> extends BaseSerde<T> {
    public BSONSerde(Class<T> clazz) {
        super(new BSONSerializer<>(clazz), new BSONDeserializer<>(clazz));
    }
}
