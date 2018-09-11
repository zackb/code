package net.jeedup.common.json;

import com.fasterxml.jackson.annotation.JsonAutoDetect;
import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.PropertyAccessor;
import com.fasterxml.jackson.core.JsonGenerator;
import com.fasterxml.jackson.databind.DeserializationFeature;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.PropertyNamingStrategy;
import com.fasterxml.jackson.databind.SerializationFeature;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * JSON utilities
 * Created by zack on 5/16/17.
 */
public class JSON {
    private static final Logger log = Logger.getLogger(JSON.class.getName());

    private static final ObjectMapper mapper;
    private static final Class MAP_CLASS;

    static {
        mapper = new ObjectMapper();
        configureMapper(mapper);
        MAP_CLASS = new HashMap<String, Object>().getClass();
    }

    public static void configureMapper(ObjectMapper objectMapper) {
        objectMapper
                .setPropertyNamingStrategy(PropertyNamingStrategy.SNAKE_CASE)
                .setVisibility(PropertyAccessor.FIELD, JsonAutoDetect.Visibility.ANY)
                .setVisibility(PropertyAccessor.GETTER, JsonAutoDetect.Visibility.NONE)
                .setVisibility(PropertyAccessor.SETTER, JsonAutoDetect.Visibility.NONE)
                .setVisibility(PropertyAccessor.CREATOR, JsonAutoDetect.Visibility.NONE)
                .setVisibility(PropertyAccessor.IS_GETTER, JsonAutoDetect.Visibility.NONE)
                .disable(SerializationFeature.FAIL_ON_EMPTY_BEANS)
                .disable(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES)
                .enable(JsonGenerator.Feature.WRITE_BIGDECIMAL_AS_PLAIN)
                .setSerializationInclusion(JsonInclude.Include.NON_NULL);
    }

    public static <T> String encode(T obj) {
        try {
            return mapper.writeValueAsString(obj);
        } catch (Exception e) {
            log.log(Level.SEVERE, "Failed encoding object: " + obj, e);
        }
        return null;
    }

    /**
     * JSON string pretty formatted
     * @param obj to format as string
     * @return prettified json string
     */
    public static <T> String pretty(T obj) {
        try {
            return mapper.writerWithDefaultPrettyPrinter().writeValueAsString(obj);
        } catch (Exception e) {
            log.log(Level.SEVERE, "Failed encoding object: " + obj, e);
        }
        return null;
    }

    public static <T> T decodeObject(String data, Class<T> clazz) {
        try {
            return mapper.readValue(data.getBytes("UTF-8"), clazz);
        } catch (Exception e) {
            log.log(Level.SEVERE, "Failed decoding object: " + clazz + " : " + data, e);
        }
        return null;
    }

    public static <T> List<T> decodeList(String data, Class<T> clazz) {
        try {
            return mapper.readValue(data, mapper.getTypeFactory().constructCollectionType(List.class, clazz));
        } catch (Exception e) {
            log.log(Level.SEVERE, "Failed decoding list: " + clazz + " : " + data, e);
        }
        return null;
    }

    @SuppressWarnings("unchecked")
    public static Map<String, Object> decode(String data) {
        return (Map<String, Object>)decodeObject(data, MAP_CLASS);
    }


	public static <T> byte[] encodeBytes(T obj) {
		try {
			return mapper.writeValueAsBytes(obj);
		} catch (Exception e) {
			log.log(Level.SEVERE, "Failed encoding object: " + obj, e);
		}
		return null;
	}

	public static <T> T decodeObject(byte[] bytes, Class<T> clazz) {
		try {
			return mapper.readValue(bytes, clazz);
		} catch (Exception e) {
			log.log(Level.SEVERE, "Failed decoding object: " + clazz + " : " + bytes, e);
		}
		return null;
	}

}
