package net.jeedup.common.math;

import java.util.HashMap;
import java.util.Map;

/**
 * Utility to keep the mode of a set of running element.
 * This class is NOT thread safe
 *
 * Created by zack on 10/4/18.
 */
public class RunningMode<T> implements Mode<T> {

    // the maximum number of elements to keep in the running mode, this is to limit the size the datastructure can use
    private static int DEFAULT_MAX_SIZE = Integer.MAX_VALUE - 1;

    private final Map<T, Long> counter;
    private final int maxSize;

    public RunningMode() {
        this(DEFAULT_MAX_SIZE);
    }

    public RunningMode(int maxSize) {
        this.maxSize = maxSize;
        counter = new HashMap<>();
    }

    public void add(T element) {
        counter.merge(element, 1L, Long::sum);
        if (counter.size() > maxSize)
            removeSmallest();
    }

    public T mode() {

        Map.Entry<T, Long> m = counter.entrySet()
                .stream()
                .max(Map.Entry.comparingByValue())
                .orElse(null);

        return m == null ? null : m.getKey();
    }

    private void removeSmallest() {
        counter.entrySet()
                .stream()
                .min(Map.Entry.comparingByValue())
                .ifPresent(m -> counter.remove(m.getKey()));

    }

    public void setDefaultMaxSize(int maxSize) {
        DEFAULT_MAX_SIZE = maxSize;
    }
}
