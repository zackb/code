package net.jeedup.common.collection.graph;

/**
 * Visitor
 * Created by zack on 3/12/16.
 */
public interface Visitor<V> {
    Visitor<V> visitTree(Tree<V> tree);
    void visitData(Tree<V> parent, V data);
}
