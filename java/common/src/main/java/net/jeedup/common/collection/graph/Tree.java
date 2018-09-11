package net.jeedup.common.collection.graph;


import java.util.LinkedHashSet;
import java.util.Set;
import java.util.stream.Stream;

/**
 * Undirected, Acyclic graph
 * Created by zack on 3/11/16.
 */
public class Tree<V> implements Visitable<V> {

    private final Tree<V> parent;

    private final Set<Tree<V>> nodes = new LinkedHashSet<>();

    public V data;

    public Tree(V data) {
        this(null, data);
    }

    public Tree(Tree<V> parent, V data) {
        this.parent = parent;
        this.data = data;
    }

    public Tree<V> child(V data) {
        for (Tree<V> child : nodes) {
            if (child.data.equals(data)) {
                return child;
            }
        }

        return child(new Tree<>(this, data));
    }

    public Tree<V> child(Tree<V> child) {
        nodes.add(child);
        return child;
    }

    public Tree<V> parent() {
        return parent;
    }

    public Set<Tree<V>> children() {
        return nodes;
    }

    @Override
    public String toString() {
        String s = " " + data;
        if (nodes != null) s += nodes.toString();
        return s;
    }

    public void breadthFirst(Consumer<V> consumer) {
        visit(new BreadthFirst<>(consumer));
    }

    public void visit(Visitor<V> visitor) {
        visitor.visitData(this, data);
        for (Tree<V> child : children()) {
            Visitor<V> childVisitor = visitor.visitTree(child);
            if (childVisitor != null) // stop?
                child.visit(childVisitor);
        }
    }

    public Stream<Tree<V>> flattened() {
        return Stream.concat(
            this.stream(),
            children()
                .stream()
                .flatMap(Tree::flattened)
        );
    }

    public Stream<Tree<V>> stream() {
        return Stream.of(this);
    }
}
