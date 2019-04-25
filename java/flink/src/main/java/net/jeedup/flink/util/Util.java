package net.jeedup.flink.util;

import static net.jeedup.common.util.Util.empty;

/**
 * Random utilities that don't have a good home. Probably break this out as this project grows (or not).
 *
 * Created by zack on 3/22/19.
 */
public class Util {

    /**
     * Try if s1 is alphanumerically greater than or equal to s2
     * @param s1 string to use for the test
     * @param s2 string to check if is greater than or equal to s2
     * @return true if s2 is alphanumerically greater than s1
     */
    public static boolean greaterThanEqual(String s1, String s2) {
        return s2.compareTo(s1) >= 0;
    }

    /**
     * Try if s1 is alphanumerically less than to s2
     * @param s1 string to use for the test
     * @param s2 string to check if is less than to s2
     * @return true if s2 is alphanumerically less than tos1
     */
    public static boolean lessThan(String s1, String s2) {
        return s2.compareTo(s1) < 0;
    }

    /**
     * Check the configured bounds to see if a file should be read.
     *
     * @param lowerBound the lowest filename alphanumerically that should be checked inclusively
     * @param higherBound the higest filename alphanumerically that should be checked exclusively
     * @param path name of the file to include or not
     * @return true if this file should be read
     */
    public static boolean checkBounds(String lowerBound, String higherBound, String path) {
        boolean pass = true;
        if (!empty(lowerBound))
            pass &= greaterThanEqual(lowerBound, path);
        if (!empty(higherBound))
            pass &= lessThan(higherBound, path);

        return pass;
    }
}
