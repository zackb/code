package net.jeedup.flink.source.io;

import net.jeedup.common.json.JSON;
import org.apache.flink.api.common.io.DelimitedInputFormat;
import org.apache.flink.api.common.typeinfo.TypeInformation;
import org.apache.flink.api.java.typeutils.ResultTypeQueryable;
import org.apache.flink.core.fs.Path;

import java.io.IOException;

/**
 * Flink InputFormat which reads delimited JSON files and parses out records using reflection.
 *
 * Created by zack on 3/19/19.
 */
public class JsonReflectInputFormat<T> extends DelimitedInputFormat<T> implements ResultTypeQueryable {

    private final Class<T> clazz;

    public JsonReflectInputFormat(Class<T> clazz, Path filePath) {
        super(filePath, null);
        this.clazz = clazz;
    }

    @Override
    public T readRecord(T reuse, byte[] bytes, int offset, int numBytes) throws IOException {
        return JSON.decodeObject(bytes, offset, numBytes, clazz);
    }

    @Override
    public TypeInformation getProducedType() {
        return TypeInformation.of(clazz);
    }
}
