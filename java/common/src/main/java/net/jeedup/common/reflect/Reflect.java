package net.jeedup.common.reflect;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

/**
 * Reflection utilities
 */
public class Reflect<T> {

    private Class<T> clazz;

    public Reflect(Class<T> clazz) {
        this.clazz = clazz;
    }

    public T instantiate(Map<String, Object> values) throws IllegalAccessException, InstantiationException {

        T obj = clazz.newInstance();

        Map<String, Field> fields = describeFields();

        for (String name : fields.keySet()) {
            setFieldValue(obj, fields.get(name), values.get(name));
        }

        return obj;
    }

    /**
     * Special handling of some type conversion
     * @param obj object instance to set field value
     * @param field to set in the object
     * @param value to set the field in the object to
     */
    private static <T> void setFieldValue(T obj, Field field, Object value) throws IllegalAccessException {
        if (value == null)
            return;

        Object interpretedValue = value;

        if (field.getType() == Long.class) {
            // small longs can be interpreted as ints
            if (value.getClass() != Long.class) {
                interpretedValue = value;
            }
        }

        field.set(obj, interpretedValue);
    }

    private static Map<Class, Map<String, Field>> fieldsCache = new ConcurrentHashMap<>();

    /**
     * Get a description of an object type's persistible fields.
     * @return map of object field names and the fields themselves
     */
    private Map<String, Field> describeFields() {
        return describeFields(clazz);
    }

    public static <T> Map<String, Field> describeFields(Class<T> clazz) {
        Map<String, Field> fields = fieldsCache.get(clazz);

        if (fields != null)
            return fields;

        fields = new LinkedHashMap<>();

        for (Field field : clazz.getDeclaredFields()) {
            int no = Modifier.ABSTRACT | Modifier.STATIC | Modifier.TRANSIENT;
            if ((field.getModifiers() & no) == 0) {
                field.setAccessible(true);
                fields.put(field.getName(), field);
            }
        }

        fieldsCache.put(clazz, fields);

        return fields;
    }

    /**
     * Gets a key-value map of the objects properties and values
     * @param obj object to convert
     * @return key-value pairs of the objects properties and values
     */
    public Map<String, Object> toMap(T obj) throws IllegalAccessException {

        Map<String, Object> result = new HashMap<>();

        Map<String, Field> fields = describeFields();

        for (String name : fields.keySet()) {
            Field field = fields.get(name);
            Object value = field.get(obj);
            if (value != null)
                result.put(name, value);
        }
        return result;
    }
}
