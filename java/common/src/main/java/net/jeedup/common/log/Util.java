package net.jeedup.common.log;

import java.util.Arrays;
import java.util.logging.Level;
import java.util.logging.LogManager;
import java.util.logging.Logger;

public class Util {

    // set the global log level
    public static void setLogLevel(final Level level) {
        Logger rootLogger  = LogManager.getLogManager().getLogger("");
        rootLogger.setLevel(level);
        Arrays.stream(rootLogger.getHandlers())
                .forEach(h -> h.setLevel(level));
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
}
