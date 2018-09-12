package net.jeedup.common.coding;

import net.jeedup.common.io.IO;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.zip.GZIPInputStream;
import java.util.zip.GZIPOutputStream;

/**
 * Gzip encoding and decoding
 */
public class GZIP {
    public static byte[] decompress(byte[] bytes) throws IOException {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        IO.copyStream(new GZIPInputStream(new ByteArrayInputStream(bytes)), out);
        return out.toByteArray();
    }

    public static byte[] compress(byte[] bytes) throws IOException {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        GZIPOutputStream gzip = new GZIPOutputStream(out);
        gzip.write(bytes);
        gzip.flush();
        gzip.close();
        return out.toByteArray();
    }
}
