package net.jeedup.common.collection.graph;

/**
 * Breadth first search a tree
 * Created by zack on 3/13/16.
 */
public class BreadthFirst<V> implements Visitor<V> {

    private final Consumer<V> consumer;
    private int level;

    public BreadthFirst(Consumer<V> consumer) {
        this(0, consumer);
    }

    public BreadthFirst(int level, Consumer<V> consumer) {
        this.level = level;
        this.consumer = consumer;
    }

    @Override
    public Visitor<V> visitTree(Tree<V> tree) {
        boolean cont = consumer.visitTree(level, tree);
        return cont ? new BreadthFirst<>(level + 1, consumer) : null;
    }
    @Override
    public void visitData(Tree<V> parent, V data) {
        consumer.visitNode(level, parent, data);
    }
}
