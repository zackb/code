package net.jeedup.app;

import net.jeedup.common.json.JSON;

import java.util.HashMap;
import java.util.Map;

public class App {
    public static void main(String[] args) {
        Map<String, String> object = new HashMap<>(1);
        object.put("message", "Hello, World!");
        System.out.println(JSON.encode(object));
    }
}
