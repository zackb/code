package net.jeedup.flink.codec;

import net.jeedup.common.json.JSON;
import org.apache.flink.api.common.serialization.Encoder;

import java.io.IOException;
import java.io.OutputStream;

/**
 * Encode an object as JSON with a newline. Using Jackson for schema.
 * Created by zack on 1/10/18.
 */
public class JsonEncoder<T> implements Encoder<T> {

    private static final byte[] NL = "\n".getBytes();

    @Override
    public void encode(T element, OutputStream stream) throws IOException {
        stream.write(JSON.encodeBytes(element));
        stream.write(NL);
    }
}
