package net.jeedup.common.collection;

import java.lang.reflect.Array;
import java.util.*;
import java.util.stream.IntStream;
import java.util.stream.Stream;

/**
 * Utility function on collections
 *
 * Created by zack on 6/14/17.
 */
public class CollectionUtil {

    public static <T> Stream<List<T>> batches(List<T> source, int length) {
        int size = source.size();
        if (size <= 0) return Stream.empty();

        int chunk = (size - 1) / length;
        return IntStream.range(0, chunk + 1).mapToObj(
                n -> source.subList(n * length, n == chunk ? size : (n + 1) * length));
    }

    public static <T> Set<T> set(T... elements) {
        return new HashSet<>(Arrays.asList(elements));
    }

    /**
     * Concatenate two arrays by creating a new array
     * @param a first array
     * @param b second array
     * @return a new array containing the values of a first and then b
     */
    public static <T> T[] concatenate(T[] a, T[] b) {
        int aLen = a.length;
        int bLen = b.length;

        @SuppressWarnings("unchecked")
        T[] c = (T[]) Array.newInstance(a.getClass().getComponentType(), aLen+bLen);
        System.arraycopy(a, 0, c, 0, aLen);
        System.arraycopy(b, 0, c, aLen, bLen);

        return c;
    }

    /**
     * Remove map entries with null values
     * @param map to mutate in place
     * @return reference to map for convenience
     */
    public static <T,A> Map<T,A> removeNull(Map<T,A> map) {
        map.entrySet().removeIf(Objects::isNull);
        return map;
    }

    /**
     * Choose an element from a collection at random
     * @param collection to choose from
     * @return a random element from the collection
     */
    public static <T> T random(Collection<T> collection) {
        int num = (int) (Math.random() * collection.size());
        for(T t : collection)
            if (--num < 0)
                return t;

        throw new AssertionError();
    }
}
