package net.jeedup.discover;

import java.util.Set;

/**
 * Created by zack on 9/13/18.
 */
public interface DiscoveryListener {
    void onHostsDiscovered(Set<String> hostnames);
}
