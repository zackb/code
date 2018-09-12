package net.jeedup.common.net;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.Enumeration;

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
}
