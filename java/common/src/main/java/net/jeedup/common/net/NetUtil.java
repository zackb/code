package net.jeedup.common.net;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;

import static net.jeedup.common.util.Util.empty;

public class NetUtil {

    // get the hostname of the local machine using the local interface
    public static String getHostName() throws UnknownHostException {
        return InetAddress.getLocalHost().getHostName();
    }

    // get the best NetworkInterface preferring non-lo
    public static NetworkInterface getNetworkInterface() throws SocketException {
        NetworkInterface networkInterface = null;
        Enumeration<NetworkInterface> interfaces = NetworkInterface.getNetworkInterfaces();

        while (interfaces.hasMoreElements()) {
            NetworkInterface inter = interfaces.nextElement();
            String name = inter.getName();
            if (networkInterface != null && !name.startsWith("lo")) {
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
     * Parse all ip addresses given a string of single hostname or ip address, or a comma separated
     * list of hostnames or ip addresses. If there are hostanmes they are resolved to all A records
     * @param subject single or comma separated list of hostnames or ip addresses
     * @return list of ip addresses
     * @throws UnknownHostException
     */
    public static List<String> resolveAddresses(String subject) throws UnknownHostException {
        List<String> result = new ArrayList<>();
        for (String host : subject.split(",")) {
            if (isHostname(host)) {
                for (InetAddress inaddr: InetAddress.getAllByName(host)) {
                    result.add(inaddr.getHostAddress());
                }
            } else {
                result.add(host);
            }
        }
        return result;
    }

    /**
     * Determine if a string is a hostanme (or ip addr)
     * RFC 1123/952 specified that hostname segments could not start with a digit.
     * @param addr possible hostname to check
     * @return true if the addr is a valid hostname
     */
    public static boolean isHostname(String addr) {
        if (empty(addr))
            return false;

        return Character.isLetter(addr.charAt(0));
    }
}
