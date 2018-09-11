package net.jeedup.common.util;

import java.util.List;
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
}
