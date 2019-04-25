package net.jeedup.common.util;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.Arrays;
import java.util.Collection;
import java.util.Enumeration;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.LogManager;
import java.util.logging.Logger;

/**
 * Random utilities
 * Created by zack on 6/7/17.
 */
public class Util {

    private static final Logger log = Logger.getLogger(Util.class.getSimpleName());

    public static boolean empty(Number number) {
        return number == null || number.equals(0);
    }

    public static boolean empty(Short number) {
        return number == null || number.equals((short)0);
    }

    public static boolean empty(Long number) {
        return number == null || number.equals(0L);
    }

    public static boolean empty(Double number) {
        return number == null || !Double.isFinite(number) || number.equals(0.0);
    }

    public static boolean empty(String string) {
        return string == null || string.length() == 0;
    }

    public static boolean empty(Collection collection) {
        return collection == null || collection.size() == 0;
    }

    public static boolean empty(Map map) {
        return map == null || map.size() == 0;
    }

    public static String getProp(String prop) {
        return getProp(prop, null);
    }

    /**
     * Get the value of an environment or -D system variable.
     * If the property name can not be found, try also the lowercase version.
     * @param prop the name of the variable to find
     * @param defaul default value to return if nothing is found
     * @return value of either the environment variable or system property of prop
     */
    public static String getProp(String prop, String defaul) {

        if (empty(prop))
            return defaul;

        String env = System.getenv(prop);
        if (empty(env))
            env = System.getProperty(prop);

        if (empty(env))
            env = System.getenv(prop.toLowerCase());

        if (empty(env))
            env = System.getProperty(prop.toLowerCase());

        if (empty(env))
            env = defaul;

        return env;
    }

    // get the hostname of the local machine using the local interface
    public static String getHostName() throws UnknownHostException {
        return InetAddress.getLocalHost().getHostName();
    }

    // get the best NetworkInterface preferring em2, em1, then non-lo
    public static NetworkInterface getNetworkInterface() throws SocketException {
        NetworkInterface networkInterface = null;
        Enumeration<NetworkInterface> interfaces = NetworkInterface.getNetworkInterfaces();

        while (interfaces.hasMoreElements()) {
            NetworkInterface inter = interfaces.nextElement();
            String name = inter.getName();
            if ("em2".equals(name)) {
                networkInterface = inter;
            } else if ((networkInterface == null || !"em2".equals(networkInterface.getName())) && "em1".equals(name)) {
                networkInterface = inter;
            } else if (networkInterface == null && !name.startsWith("lo") && inter.getHardwareAddress() != null) {
                networkInterface = inter;
            }
        }

        return networkInterface;
    }

    // get the MAC address of a network interface
    public static String getMacAddress(NetworkInterface inter) throws SocketException {
        byte[] mac = inter.getHardwareAddress();
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < mac.length; i++)
            sb.append(String.format("%02X%s", mac[i], (i < mac.length - 1) ? "-" : ""));
        return sb.toString();
    }

    /**
     * Create a unique-ish identifying string for this host (consumer).
     * Use the hostname if available else the MAC address of the em2, em1, non-lo interfaces
     * @return clientId
     */
    public static String createClientId(String prefix) {
        String clientId = null;
        try {
            clientId = Util.getHostName();
        } catch (UnknownHostException e) {
            log.warning("Failed getting hostname for clientId, trying MAC");
        }

        if (empty(clientId)) {
            try {
                NetworkInterface inter = Util.getNetworkInterface();
                if (inter != null)
                    clientId = Util.getMacAddress(inter);
            } catch (SocketException e) {
                log.severe("Failed getting MAC address for clientId, defaulting to 'UNKNOWN'");
            }
        }

        if (empty(clientId)) {
            clientId = "UNKNOWN";
        }

        clientId = prefix + "_" + clientId.replaceAll("[^A-Za-z0-9]", "_");

        return clientId;
    }

    // get a JUL Level from a string
    public static Level logLevel(String level) {
        Level l = null;
        switch (level.toLowerCase()) {
            case "severe":
                l = Level.SEVERE;
                break;
            case "warning":
                l = Level.WARNING;
                break;
            case "info":
                l  = Level.INFO;
                break;
            case "config":
                l  = Level.CONFIG;
                break;
            case "fine":
                l  = Level.FINE;
                break;
            case "finer":
                l  = Level.FINER;
                break;
            case "finest":
                l  = Level.FINEST;
                break;
            case "all":
                l  = Level.ALL;
                break;
            default:
                break;
        }
        return l;
    }

    // set the global log level
    public static void setLogLevel(final Level level) {
        Logger rootLogger  = LogManager.getLogManager().getLogger("");
        rootLogger.setLevel(level);
        Arrays.stream(rootLogger.getHandlers())
                .forEach(h -> h.setLevel(level));
    }
}
