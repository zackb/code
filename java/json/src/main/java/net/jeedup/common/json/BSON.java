package net.jeedup.common.json;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.dataformat.smile.SmileFactory;
import net.jeedup.common.util.GZIP;

import java.io.IOException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * BSON (Binary JSON) based encoding and decoding
 * http://bsonspec.org/
 */
public class BSON {

    private static final Logger log = Logger.getLogger(BSON.class.getName());

    private static final ObjectMapper mapper;

    private static final Class mapClass;

    static {
        mapper = new ObjectMapper(new SmileFactory());
        JSON.configureMapper(mapper);

        mapClass = HashMap.class;
    }

    public static byte[] encode(Object obj) {
        return encode(obj, false);
    }

    public static byte[] encode(Object obj, boolean compress) {

        byte[] bytes = null;

        try {
            bytes = mapper.writeValueAsBytes(obj);
            if (compress)
                bytes = GZIP.compress(bytes);
        } catch (IOException e) {
            log.log(Level.SEVERE, "Failed encoding BSON", e);
        }

        return bytes;
    }

    public static <T> T decodeObject(byte[] bytes, Class<T> clazz) {
        return decodeObject(bytes, clazz, false);
    }

    public static <T> T decodeObject(byte[] bytes, Class<T> clazz, boolean compressed) {
        try {
            if (compressed) {
                bytes = GZIP.decompress(bytes);
            }

            return mapper.readValue(bytes, clazz);
        } catch (IOException e) {
            log.log(Level.SEVERE, "Failed decoding BSON", e);
        }
        return null;
    }

    public static Map<String, Object> decode(byte[] bytes) {
        return decode(bytes, false);
    }

    public static Map<String, Object> decode(byte[] bytes, boolean compressed) {
        return (Map<String, Object>) decodeObject(bytes, mapClass, compressed);
    }

    public static <T> List<T> decodeList(String data, Class<T> clazz) {
        try {
            return mapper.readValue(data, mapper.getTypeFactory().constructCollectionType(List.class, clazz));
        } catch (IOException e) {
            log.log(Level.SEVERE, "Failed decoding BSON List", e);
        }
        return null;
    }
}
