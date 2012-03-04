package com.jeedup.util

/**
 * A map that will cache a value in memory for a given time interval.
 * When the value expires or is not present in the map the provided
 * Closure will be asked to (re)load the data.
 *
 * User: zack
 * Date: 2/17/12
 */
class CacheMap<K, V> implements Map<K, V>   {

    private List<CacheMapEntry> values = []

    private final long expire

    private Closure loader

    private CacheMap()    {

    }

    /**
     * Create a new map of cached values
     *
     * @param callable closure that will load data, accepts one K and returns one V
     * @param expire millisecods that a value is valid for before reloading it
     */
    public CacheMap(Closure callable, long expire)   {
        this.loader = callable
        this.expire = expire
    }

    public int size()   {
        return values.size()
    }

    public boolean isEmpty()    {
        return values.isEmpty()
    }

    public boolean containsKey(Object k)   {
        return values*.key.contains(k)
    }

    boolean containsValue(Object v)    {
        return values*.value.contains(v)
    }

    public synchronized V get(Object k) {
        for (CacheMapEntry<K, V> entry : values)    {
            if (entry.key.equals(k))    {
                if (entry.isExpired())  {
                    return load((K)k)
                }
                return entry.value
            }
        }

        return load((K)k)
    }

    public synchronized V put(K k, V v)  {
        for (CacheMapEntry<K, V> entry : values)    {
            if (entry.key.equals(k))    {
                entry.value = v
                entry.expire = System.currentTimeMillis() + expire
                return entry.value
            }
        }

        CacheMapEntry<K, V> entry = new CacheMapEntry<K, V>(k, v)
        entry.expire = System.currentTimeMillis() + expire
        values.add(entry)

        return entry.value
    }

    public synchronized V remove(Object k)   {
        for (CacheMapEntry<K, V> entry : values)    {
            if (entry.key.equals(k))    {
                values.remove(entry)
                return entry.value
            }
        }

        return null
    }

    public void putAll(Map<? extends K, ? extends V> map)   {
        map.each { K k, V v ->
            put(k, v)
        }
    }

    public void clear()    {
        values.clear()
    }

    public Set<K> keySet()  {
        return new HashSet<K>(values*.key as List<K>)
    }

    public Collection<V> values()   {
        return new ArrayList<V>(values*.value as List<V>)
    }

    public Set<Map.Entry<K, V>> entrySet() {
        return new HashSet<Map.Entry<K,V>>(values)
    }
    
    private V load(K key)   {
        V value = (V)loader.call(key)
        put(key, value)
        return value
    }

    public class CacheMapEntry<K, V> implements Map.Entry<K, V> {
        public long expire
        private final K k
        private V v

        public CacheMapEntry(K key, V value)    {
            k = key
            v = value
        }

        public K getKey()  {
            return k
        }

        public V setValue(V value)  {
            v = value
            return v
        }

        public V getValue()  {
            return v
        }

        public boolean isExpired()  {
            if (System.currentTimeMillis() > expire)   {
                return true
            }

            return false
        }
    }

    public static void main(String[] args) {
        CacheMap<String, Integer> map = new CacheMap<String, Integer>({ String key ->
            return key.toInteger()
        }, 200)

        for (int i = 0; i < 100; i++) {
            Thread.start {
                map.put("1", 1)
                println map.get("1")
                println map.get("2")
                Thread.sleep(100)
                //map.remove("1")
                println map.get("1")
                println map.get("2")
                map.remove("FFDS")
                println "Size: ${map.size()}"
            }
        }
    }
}
