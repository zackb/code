package net.jeedup.common.collection.graph;

/**
 * Created by zack on 3/13/16.
 */

public interface Consumer<V> {
    boolean visitTree(int level, Tree<V> tree);
    boolean visitNode(int level, Tree<V> parent, V data);
}
