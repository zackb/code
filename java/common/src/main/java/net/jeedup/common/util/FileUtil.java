package net.jeedup.common.util;

import java.io.IOException;
import java.nio.file.*;
import java.nio.file.attribute.BasicFileAttributes;

import static net.jeedup.common.util.Util.empty;

/**
 * Utilities for working with files and paths
 *
 * Created by zack on 7/10/17.
 */
public class FileUtil {

    /**
     * Attempt to parse the file extension
     * @param filename path or full host + path to parse
     * @return file extension or empty string on failure
     */
    public static String parseFileExtension(String filename) {
        if (empty(filename)) return "";

        String ext = "";
        String path = null;

        // get the path without fragments or query params
        Path fname = Paths.get(filename).getFileName();
        if (fname != null)
            path = fname.toString();

        if (empty(path)) return ext;

        // if there's no file extension dont try to guess
        if (!path.contains(".")) return ext;

        // trim uri fragment if applicable
        if (path.contains("#"))
            path = path.substring(0, path.indexOf("#"));

        // trim uri query params
        if (path.contains("?"))
            path = path.substring(0, path.indexOf("?"));

        // get the file extension
        ext = path.substring(path.lastIndexOf(".") + 1);

        return ext;
    }

    /**
     * Delete a file or directory recursively
     * @param filePath file or directory path to delete
     * @throws IOException on problem accessing the path
     */
    public static void delete(String filePath) throws IOException {
        delete(Paths.get(filePath));
    }

    /**
     * Delete a file or directory recursively, swallowing IOExceptions
     * @param path file or directory path to delete
     */
    public static void deleteEx(Path path) {
        try {
            delete(path);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Delete a file or directory recursively
     * @param path file or directory path to delete
     * @throws IOException on problem accessing the path
     */
    public static void delete(Path path) throws IOException {
        if (!Files.exists(path)) return;
        if (!Files.isDirectory(path)) {
            Files.delete(path);
        } else {
            Files.walkFileTree(path, DeleteRecursively);
        }
    }

    private static final FileVisitor<Path> DeleteRecursively = new SimpleFileVisitor<Path>() {
        @Override
        public FileVisitResult visitFile(Path file, BasicFileAttributes attrs) throws IOException {
            if (Files.exists(file))
                Files.delete(file);
            return FileVisitResult.CONTINUE;
        }

        @Override
        public FileVisitResult postVisitDirectory(Path dir, IOException exc) throws IOException {
            if (Files.exists(dir))
                Files.delete(dir);
            return FileVisitResult.CONTINUE;
        }
    };


    /**
     * Get the default system temp dir.
     * @return the detault OS tmp dir with trailing '/'
     */
    public static String tempDir() {
        String tmp = System.getProperty("java.io.tmpdir");
        if (empty(tmp))
            tmp = "/tmp/";
        if (!tmp.endsWith("/"))
            tmp = tmp + "/";
        return tmp;
    }
}
