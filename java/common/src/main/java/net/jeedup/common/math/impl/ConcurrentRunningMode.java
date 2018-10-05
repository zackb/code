package net.jeedup.common.math.impl;

import net.jeedup.common.math.Mode;

import java.util.Comparator;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.LongAdder;

/**
 * Thread safe mode calculator.
 *
 * Created by zack on 10/4/18.
 */
public class ConcurrentRunningMode<T> implements Mode<T> {

    private final Map<T, LongAdder> counter = new ConcurrentHashMap<>();

    @Override
    public void add(T element) {
        counter.computeIfAbsent(element, k -> new LongAdder()).increment();
    }

    @Override
    public T mode() {
        Map.Entry<T, LongAdder> m = counter.entrySet()
                .stream()
                .max(Comparator.comparingLong((Map.Entry<T, LongAdder> e) -> e.getValue().longValue()))
                .orElse(null);

        return m == null ? null : m.getKey();
    }
}
