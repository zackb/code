package net.jeedup.common.net;

import net.jeedup.common.io.IO;

import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.zip.GZIPInputStream;

/**
 * Utility class to make simple HTTP requests
 *
 * Created by zack on 1/17/18.
 */
public class HTTP {

    /**
     * Perform a HTTP GET request to the provided uri and read the response
     * @param uri to request
     * @return response body as string
     * @throws IOException on non-2xx status or connection failure
     */
    public static String get(String uri) throws IOException {
        HttpURLConnection connection = (HttpURLConnection) new URL(uri).openConnection();
        connection.setRequestProperty("Content-Type", "application/json;charset=UTF-8");
        return IO.readString(connection.getInputStream());
    }

    public static String getInput(HttpURLConnection connection) throws IOException {
        InputStream ins = null;
        if ("gzip".equals(connection.getContentEncoding()))
            ins = new GZIPInputStream(connection.getInputStream());
        else
            ins = connection.getInputStream();
        return IO.readString(ins);
    }

    public static String getError(HttpURLConnection connection) throws IOException {
        InputStream ins = null;
        if ("gzip".equals(connection.getContentEncoding()))
            ins = new GZIPInputStream(connection.getErrorStream());
        else
            ins = connection.getErrorStream();
        return IO.readString(ins);
    }
}
