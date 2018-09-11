package net.jeedup.common.collection;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

/**
 * List which can not be changed after creation using {@link #from}
 * Created by zack on 7/3/17.
 */
public class ImmutableList<T> extends ArrayList<T> {

    private ImmutableList() { }

    private ImmutableList(Collection<T> data) {
        super(data);
    }

    /**
     * Construct an unmodifiable list from a given base collection
     * @param data to create the list with
     * @return unmodifiable list containing supplied data
     */
    public static <T> List<T> from(Collection<T> data) {
        return new ImmutableList<>(data);
    }

    @Override
    public boolean add(T object) {
        throw new UnsupportedOperationException();
    }

    @Override
    public boolean addAll(Collection coll) {
        throw new UnsupportedOperationException();
    }

    @Override
    public void clear() {
        throw new UnsupportedOperationException();
    }


    @Override
    public T remove(int index) {
        throw new UnsupportedOperationException();
    }

    @Override
    public boolean remove(Object object) {
        throw new UnsupportedOperationException();
    }

    @Override
    public boolean removeAll(Collection coll) {
        throw new UnsupportedOperationException();
    }

    @Override
    public boolean retainAll(Collection coll) {
        throw new UnsupportedOperationException();
    }

    @Override
    public void add(int index, T object) {
        throw new UnsupportedOperationException();
    }

    @Override
    public boolean addAll(int index, Collection coll) {
        throw new UnsupportedOperationException();
    }

    @Override
    public T set(int index, T object) {
        throw new UnsupportedOperationException();
    }
}
