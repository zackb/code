package net.jeedup.discover;

import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;

import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.CountDownLatch;

import static net.jeedup.common.collection.CollectionUtil.set;
import static org.junit.jupiter.api.Assertions.*;

class DiscoverTest {

    @Test
    void testNoneMyself() throws Exception {

        CountDownLatch latch = new CountDownLatch(2);

        Discover discover1 = new Discover("testCluster", hostnames -> {
            System.out.println("Found1: " + hostnames);
            assertEquals(0, hostnames.size());
            latch.countDown();
        });

        Discover discover2 = new Discover("testCluster", hostnames -> {
            System.out.println("Found2: " + hostnames);
            assertEquals(0, hostnames.size());
            latch.countDown();
        });

        discover1.start();
        discover2.start();

        latch.await();

        discover1.stop();
        discover2.stop();

        CountDownLatch l1 = new CountDownLatch(2);

        discover1 = new Discover("testCluster", "id1", hostnames -> {
            System.out.println("Found1: " + hostnames);
            assertEquals(0, hostnames.size());
            l1.countDown();
        });

        discover2 = new Discover("testCluster", "id1", hostnames -> {
            System.out.println("Found2: " + hostnames);
            assertEquals(0, hostnames.size());
            l1.countDown();
        });

        discover1.start();
        discover2.start();

        l1.await();

        discover1.stop();
        discover2.stop();
    }

    @Disabled
    @Test
    void testMyselfIdentifier() throws Exception {

        CountDownLatch latch = new CountDownLatch(2);

        Set<String> hosts = new HashSet<>();

        Discover discover1 = new Discover("testCluster", "id1", hostnames -> {
            hosts.addAll(hostnames);
            latch.countDown();
        });

        Discover discover2 = new Discover("testCluster", "id2", hostnames -> {
            hosts.addAll(hostnames);
            latch.countDown();
        });

        discover1.start();
        discover2.start();

        latch.await();

        Thread.sleep(10000L);

        discover1.stop();
        discover2.stop();

        assertEquals(set("foo"), hosts);
    }
}