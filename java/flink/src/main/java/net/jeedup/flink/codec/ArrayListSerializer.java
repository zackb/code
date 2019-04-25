package net.jeedup.flink.codec;

import com.esotericsoftware.kryo.serializers.CollectionSerializer;
import com.esotericsoftware.kryo.Kryo;
import com.esotericsoftware.kryo.io.Input;

import java.util.ArrayList;
import java.util.Collection;

/**
 *
 * Created by zack on 3/8/19.
 */
public class ArrayListSerializer extends CollectionSerializer {
    public ArrayListSerializer() {super();}
    protected Collection create(Kryo kryo, Input input, Class<Collection> type) {
        return new ArrayList();
    }
}
