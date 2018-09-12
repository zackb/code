package net.jeedup.message.serde.json;

import net.jeedup.message.impl.BaseSerde;

/**
 * TODO
 * Created by zack on 9/11/18.
 */
public class JSONSerde<T> extends BaseSerde<T> {

    public JSONSerde(Class<T> clazz) {
        super(new JSONSerializer<>(clazz), new JSONDeserializer<>(clazz));
    }
}