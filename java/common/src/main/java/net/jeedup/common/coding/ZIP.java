package net.jeedup.common.coding;

import net.jeedup.common.io.IO;

import java.io.*;
import java.util.Enumeration;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import java.util.zip.ZipOutputStream;

/**
 * Zip utilities
 */
public class ZIP {

    public static void compressDir(String dir, String archive) throws IOException {
        FileOutputStream fout = new FileOutputStream(archive);
        ZipOutputStream zip = new ZipOutputStream(fout);
        addFolderToZip("", dir, zip);
        IO.close(zip);
        IO.close(fout);
    }

    private static void addFolderToZip(String path, String srcFolder, ZipOutputStream zip) throws IOException {
        File dir = new File(srcFolder);
        if (dir.list().length == 0) {
            addFileToZip(path , srcFolder, zip, true);
        } else {
            for (String fileName : dir.list()) {
                if (path.equals("")) {
                    addFileToZip(dir.getName(), srcFolder + '/' + fileName, zip, false);
                } else {
                    addFileToZip(path + '/' + dir.getName(), srcFolder + '/' + fileName, zip, false);
                }
            }
        }
    }

    private static void addFileToZip(String path, String srcFile, ZipOutputStream zip, boolean flag) throws IOException {
        File dir = new File(srcFile);
        if (flag) {
            zip.putNextEntry(new ZipEntry(path + '/' + dir.getName() + '/'));
        } else {
            if (dir.isDirectory()) {
                addFolderToZip(path, srcFile, zip);
            } else {
                byte[] buf = new byte[1024];
                int len;
                FileInputStream fin = new FileInputStream(srcFile);
                zip.putNextEntry(new ZipEntry(path + '/' + dir.getName()));
                while ((len = fin.read(buf)) > 0) {
                    zip.write(buf, 0, len);
                }
            }
        }
    }

    public static void decompressDir(String destDir, String archive) throws IOException {
        File file = new File(archive);
        ZipFile zip = new ZipFile(file);
        new File(destDir).mkdir();
        Enumeration zipFileEntries = zip.entries();
        while (zipFileEntries.hasMoreElements()) {
            ZipEntry entry = (ZipEntry) zipFileEntries.nextElement();
            String currentEntry = entry.getName();
            File destFile = new File(destDir, currentEntry);
            File destinationParent = destFile.getParentFile();

            destinationParent.mkdirs();

            if (!entry.isDirectory()) {
                BufferedInputStream is = new BufferedInputStream(zip.getInputStream(entry));
                // write the current file to disk
                FileOutputStream fos = new FileOutputStream(destFile);
                BufferedOutputStream dest = new BufferedOutputStream(fos);
                IO.copyStream(is, dest);
                IO.close(dest);
            }
        }
    }
}
