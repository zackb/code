package net.jeedup.flink.config.aws;

import java.io.Serializable;

import static net.jeedup.common.util.Util.empty;
import static net.jeedup.common.util.Util.getProp;

/**
 * Common configuration for AWS stuff.
 *
 * Created by zack on 3/12/19.
 */
public class AwsConfig implements Serializable {

    public String region = "us-west-2";
    public String accessKey;
    public String secretKey;

    public static AwsConfig createFromEnv() {
        AwsConfig config = new AwsConfig();
        config.region = getProp("AWS_REGION", config.region);
        if (empty(config.region)) {
            config.region = getProp("AWS_DEFAULT_REGION");
        }
        config.accessKey = getProp("AWS_ACCESS_KEY_ID");
        config.secretKey = getProp("AWS_SECRET_ACCESS_KEY");
        return config;
    }
}
