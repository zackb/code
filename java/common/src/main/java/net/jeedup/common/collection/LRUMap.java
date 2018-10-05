package net.jeedup.common.collection;

import java.util.LinkedHashMap;
import java.util.Map;

/**
 * A Map that implements least recently used for decaying.
 * Objects are aged out of the map based on LRU.
 */
public class LRUMap<K, V> extends LinkedHashMap<K, V> {

    private int maxSize;

    public LRUMap(int maxSize) {
        super(10, 1.0f, true);
        this.maxSize = maxSize;
    }

    @Override
    protected boolean removeEldestEntry(final Map.Entry<K, V> eldest) {
        return size() > maxSize;
    }
}
