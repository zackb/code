package net.jeedup.common.util;

/**
 * Various utilites for working with numbers
 * Created by zack on 1/25/18.
 */
public class NumberUtil {

    /**
     * Convert a string into an integer using Integer.parseInt swallowing runtime exceptions
     * @param s string to convert
     * @return integer conversion or null if the string is not an integer
     */
    public static Integer parseInt(String s) {
        try {
            return Integer.parseInt(s);
        } catch (NumberFormatException e) {
            System.err.println("NumberFormatException: " + s + e.getMessage());
        }
        return null;
    }

    /**
     * Convert a string into a long using Long.parseLong swallowing runtime exceptions
     * @param s string to convert
     * @return long conversion or null if the string is not a long
     */
    public static Long parseLong(String s) {
        try {
            return Long.parseLong(s);
        } catch (NumberFormatException e) {
            System.err.println("NumberFormatException: " + s + e.getMessage());
        }
        return null;
    }
}
