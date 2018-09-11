package net.jeedup.common.util;

import java.util.Collection;
import java.util.Map;

/**
 * Random utilites
 * Created by zack on 6/7/17.
 */
public class Util {

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
}
