package net.jeedup.message.serde.json;

import net.jeedup.common.json.JSON;
import net.jeedup.message.impl.BaseDeserializer;

/**
 * TODO
 * Created by zack on 9/11/18.
 */
public class JSONDeserializer<T> extends BaseDeserializer<T> {

    public JSONDeserializer(Class<T> clazz) {
        super(clazz);
    }

    @Override
    public T deserialize(byte[] bytes) {
        return JSON.decodeObject(bytes, clazz);
    }
}
