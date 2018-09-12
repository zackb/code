package net.jeedup.message.broker;

import net.jeedup.common.net.NetUtil;

import java.net.NetworkInterface;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.logging.Logger;

import static net.jeedup.common.util.Util.empty;

public class BrokerUtil {

    private static final Logger log = Logger.getLogger(BrokerUtil.class.getName());

    /**
     * Create a unique-ish identifying string for this host (consumer).
     * Use the hostname if available else the MAC address of the non-lo interfaces
     * @return clientId
     */
    public static String createClientId() {
        String clientId = null;
        try {
            clientId = NetUtil.getHostName();
        } catch (UnknownHostException e) {
            log.warning("Failed getting hostname for clientId, trying MAC");
        }

        if (empty(clientId)) {
            try {
                NetworkInterface inter = NetUtil.getNetworkInterface();
                if (inter != null)
                    clientId = NetUtil.getMacAddress(inter);
            } catch (SocketException e) {
                log.severe("Failed getting MAC address for clientId, defaulting to 'UNKNOWN'");
            }
        }

        if (empty(clientId)) {
            clientId = "UNKNOWN";
        }

        clientId = "jeedup_" + clientId.replaceAll("[^A-Za-z0-9]", "_");

        return clientId;
    }
}
