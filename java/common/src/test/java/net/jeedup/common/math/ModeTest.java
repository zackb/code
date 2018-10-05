package net.jeedup.common.math;

import net.jeedup.common.math.impl.ConcurrentRunningMode;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

class ModeTest {

    @Test
    void testNoElements() {
        assertNull(new RunningMode<>().mode());
        assertNull(new ConcurrentRunningMode<>().mode());
    }

    @Test
    void testMode() {
        modeTest(new RunningMode<>());
        modeTest(new ConcurrentRunningMode<>());
    }

    @Test
    void testRotate() {
        Mode<Integer> mode = new RunningMode<>(3);
        mode.add(54);
        mode.add(45);
        mode.add(54);
        mode.add(32);
        assertEquals(54, mode.mode().intValue());
        mode.add(54);
        mode.add(54);
        mode.add(54);
        mode.add(54);
        mode.add(54);
        mode.add(31);
        mode.add(30);
        mode.add(31);
        mode.add(23);
        mode.add(23);
        mode.add(22);
        mode.add(21);
        assertEquals(54, mode.mode().intValue());
    }

    void modeTest(Mode<String> mode) {
        mode.add("Hello");
        mode.add("World");
        mode.add("Foo");
        assertNotNull(mode.mode());
        mode.add("World");
        assertEquals("World", mode.mode());
        mode.add("Foo");
        mode.add("Foo");
        assertEquals("Foo", mode.mode());
    }
}