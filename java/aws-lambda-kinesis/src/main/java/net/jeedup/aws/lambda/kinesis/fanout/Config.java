package net.jeedup.aws.lambda.kinesis.fanout;

import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

import static net.jeedup.common.util.Util.empty;

/**
 * Fanout lambda config
 * Created by zack on 9/15/18.
 */
public class Config {

    private static final String ENV_TARGET_STREAMS = "TARGET_STREAMS";

    public final Set<String> targetStreamNames;

    public Config() {
        String targets = getEnvOrThrow(ENV_TARGET_STREAMS);
        targetStreamNames = new HashSet<>(Arrays.asList(targets.split(",")));
    }

    private static String getEnvOrThrow(String name) throws RuntimeException {
        String result = System.getenv(name);
        if (empty(result)) throw new RuntimeException("No " + name + " ENV var defined");
        return result;
    }
}
