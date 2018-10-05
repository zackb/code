package net.jeedup.common.math;

/**
 * Utility to keep the mode of some element.
 *
 * Created by zack on 10/4/18.
 */
public interface Mode<T> {
    void add(T element);
    T mode();
}
