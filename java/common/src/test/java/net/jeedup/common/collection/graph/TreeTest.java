package net.jeedup.common.collection.graph;


import org.junit.jupiter.api.Test;

import java.util.Arrays;

import static java.util.stream.Collectors.toList;
import static org.junit.jupiter.api.Assertions.*;

/**
 * Created by zack on 3/11/16.
 */
class TreeTest {

    @Test
    void testSimple() {
        Tree<String> tree = new Tree<>("root");

        tree.child("branch1").data = "leaf1";
        tree.child("branch1").child("branch2").data = "leaf2";

        assertEquals(Arrays.asList("root", "leaf1", "branch1", "leaf2"),
            tree.flattened().map(t -> t.data)
                    .collect(toList()));

        assertEquals(" root[ leaf1[],  branch1[ leaf2[]]]", tree.toString());
    }
}
