package net.jeedup.discover;

import net.jeedup.common.net.NetUtil;
import net.jeedup.common.util.UUIDs;
import org.jgroups.conf.ClassConfigurator;
import org.jgroups.util.ExtendedUUID;

import java.net.UnknownHostException;
import java.util.UUID;
import java.util.function.Supplier;

import static net.jeedup.common.util.Util.empty;

/**
 * TODO
 * Created by zack on 9/13/18.
 */
public class HostnameAddress extends ExtendedUUID {

    private static final UUID uuid;
    private static final String hostname;

    static {
        uuid = UUIDs.random();
        hostname = createHostname();
        ClassConfigurator.add((short)5454,HostnameAddress.class);
    }

    private static final String KEY_HOSTNAME = "n";
    private static final String KEY_IDENT    = "i";


    public HostnameAddress() {
        this(null);
    }

    public HostnameAddress(String identifier) {
        super(uuid.getMostSignificantBits(), uuid.getLeastSignificantBits());
        put(KEY_HOSTNAME, hostname.getBytes());
        if (!empty(identifier)) {
            put(KEY_IDENT, identifier.getBytes());
        }
    }

    public String getHostname() {
        return new String(get(KEY_HOSTNAME));
    }

    public String getIdentifier() {
        String result = null;
        byte[] bytes = get(KEY_IDENT);
        if (bytes != null)
            result = new String(bytes);
        return result;
    }

    @Override
    public Supplier<? extends org.jgroups.util.UUID> create() {
        return HostnameAddress::new;
    }

    private static String createHostname() {
        String result = "unknown";
        try {
            result = NetUtil.getHostName();
        } catch (UnknownHostException e) {
            // TODO:
            e.printStackTrace();
        }
        return result;
    }
}
