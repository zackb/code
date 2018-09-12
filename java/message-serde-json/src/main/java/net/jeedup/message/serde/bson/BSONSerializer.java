package net.jeedup.message.serde.bson;

import net.jeedup.common.json.BSON;
import net.jeedup.message.impl.BaseSerializer;

/**
 * TODO
 * Created by zack on 9/11/18.
 */
public class BSONSerializer<T> extends BaseSerializer<T> {

    public BSONSerializer(Class<T> clazz) {
        super(clazz);
    }

    @Override
    public byte[] serialize(T obj) {
        return BSON.encode(obj);
    }
}
