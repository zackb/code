package net.jeedup.message.serde.json;

import net.jeedup.common.json.JSON;
import net.jeedup.message.impl.BaseSerializer;

/**
 * TODO
 * Created by zack on 9/11/18.
 */
public class JSONSerializer<T> extends BaseSerializer<T> {

    public JSONSerializer(Class<T> clazz) {
        super(clazz);
    }

    @Override
    public byte[] serialize(T obj) {
        return JSON.encodeBytes(obj);
    }
}
