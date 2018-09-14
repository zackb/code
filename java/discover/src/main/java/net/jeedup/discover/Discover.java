package net.jeedup.discover;

import org.jgroups.*;

import java.util.HashSet;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;

import static net.jeedup.common.util.Util.empty;

/**
 * TODO
 * Created by zack on 9/13/18.
 */
public class Discover extends ReceiverAdapter {

    private static final Logger log = Logger.getLogger(Discover.class.getSimpleName());

    private final String clusterName;

    private final HostnameAddress address;
    private final String identifier;

    private JChannel channel;
    private final DiscoveryListener listener;

    public Discover(String clusterName, DiscoveryListener listener) {
        this(clusterName, null, listener);
    }

    public Discover(String clusterName, String identifier, DiscoveryListener listener) {
        this.address = new HostnameAddress(identifier);
        this.clusterName = clusterName;
        this.listener = listener;
        this.identifier = identifier;
    }

    public void start() throws Exception {

        if (channel != null && !channel.isClosed())
            throw new Exception("Channel is already open");

        channel = new JChannel();
        channel.setReceiver(this);
        channel.addAddressGenerator(() -> address);
        channel.setDiscardOwnMessages(true);
        channel.connect(clusterName);
    }

    public void stop() {
        if (channel != null)
            channel.close();
        channel = null;
    }

    @Override
    public void viewAccepted(View view) {

        Set<String> hosts = new HashSet<>();

        for (Address remote : view.getMembers()) {
            if (remote instanceof HostnameAddress) {
                HostnameAddress ha = (HostnameAddress) remote;
                // weed out myself
                if (!isMe(ha)) {
                    hosts.add(ha.getHostname());
                }
            } else {
                log.log(Level.SEVERE, "Received an unknown address type: " +
                        remote.getClass().getName() + " : " + remote.toString());
            }
        }
        listener.onHostsDiscovered(hosts);
    }

    boolean isMe(HostnameAddress remote) {
        boolean result = false;
        if (address.getHostname().equals(remote.getHostname())) {
            result = true;
            String remoteId = remote.getIdentifier();
            if (!empty(identifier) && !empty(remoteId)) {
                result = identifier.equals(remoteId);
            }
        }
        return result;
    }

    @Override
    public void receive(Message msg) {
        log.info("I received a message, this is weird: " + msg.getSrc() + ": " + msg.getObject());
    }
}
