package net.jeedup.common.net;

/**
 * Holds hostnames and ports
 * Created by zack on 9/12/18.
 */
public class HostAndPort {
    public String host;
    public Integer port;

    public HostAndPort() { }

    public HostAndPort(String host, Integer port) {
        this.host = host;
        this.port = port;
    }
}
