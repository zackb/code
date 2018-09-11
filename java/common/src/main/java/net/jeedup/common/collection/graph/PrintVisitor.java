package net.jeedup.common.collection.graph;

/**
 * System.out.println each data
 * Created by zack on 3/13/16.
 */
public class PrintVisitor<V> implements Visitor<V> {

    private final int indent;

    public PrintVisitor(int indent) {
        this.indent = indent;
    }

    @Override
    public Visitor<V> visitTree(Tree<V> tree) {
        return new PrintVisitor<>(indent + 2);
    }

    @Override
    public void visitData(Tree<V> parent, V data) {
        for (int i = 0; i < indent; i++) System.out.print(" ");
        System.out.println(data);
    }
}
