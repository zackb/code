package net.jeedup.common.util;

import java.util.Collection;
import java.util.Map;
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
}
