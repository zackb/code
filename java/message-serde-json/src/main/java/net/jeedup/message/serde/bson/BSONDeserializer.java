package net.jeedup.message.serde.bson;

import net.jeedup.common.json.BSON;
import net.jeedup.message.impl.BaseDeserializer;

/**
 * TODO
 * Created by zack on 9/11/18.
 */
public class BSONDeserializer<T> extends BaseDeserializer<T> {

    public BSONDeserializer(Class<T> clazz) {
        super(clazz);
    }

    @Override
    public T deserialize(byte[] bytes) {
        return BSON.decodeObject(bytes, clazz);
    }
}
