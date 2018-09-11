package net.jeedup.common.collection;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

/**
 * Map which can not be changed after creation using a builder.
 * Created by zack on 5/10/18.
 */
public class ImmutableMap<K, V> extends HashMap<K, V> {

    @Override
    public boolean remove(Object key, Object value) {
        throw new UnsupportedOperationException();
    }

    @Override
    public V put(K key, V value) {
        throw new UnsupportedOperationException();
    }

    public static <K, V> Builder<K, V> builder() {
        return new Builder<>();
    }

    public static <K, V> Builder<K, V> builder(int capacity) {
        return new Builder<>(capacity);
    }

    public static class Builder<K, V> {

        private final Map<K, V> map;

        public Builder() {
            map = new HashMap<>();
        }

        public Builder(int capacity) {
            map = new HashMap<>(capacity);
        }

        public Builder<K,V> put(K key, V value) {
            map.put(key, value);
            return this;
        }

        public Map<K, V> build() {
            return Collections.unmodifiableMap(map);
        }
    }
}
