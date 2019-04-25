package net.jeedup.flink.codec;

import org.apache.avro.Schema;
import org.apache.avro.generic.GenericData;
import org.apache.avro.reflect.ReflectData;
import org.apache.flink.formats.parquet.ParquetBuilder;
import org.apache.flink.formats.parquet.ParquetWriterFactory;
import org.apache.parquet.avro.AvroParquetWriter;
import org.apache.parquet.hadoop.ParquetWriter;
import org.apache.parquet.hadoop.metadata.CompressionCodecName;
import org.apache.parquet.io.OutputFile;

import java.io.IOException;

/**
 * Utilities for working with Parquet sources and sinks. Copy/pasted code until I find an appropriate home for them.
 *
 * Created by zack on 3/19/19.
 */
public class ParquetWriterSink {

    /**
     * Create a Parquet data writer which uses Avro for schema and Snappy for compression.
     * @param schemaString avro schema
     * @param model avro data model
     * @param out the file output stream to write to
     * @param <T> generic class which will be written
     * @return a new data writer which will write parquet formatted data compressed with Snappy
     */
    private static <T> ParquetWriter<T> createAvroParquetWriter(String schemaString, GenericData model, OutputFile out) throws IOException {
        Schema schema = (new Schema.Parser()).parse(schemaString);
        return AvroParquetWriter
                .<T>builder(out)
                .withSchema(schema)
                .withDataModel(model)
                .withCompressionCodec(CompressionCodecName.SNAPPY)
                .build();
    }

    /**
     * Create a writer using reflection for avro schema discovery.
     * @param type java class to reflect for schema discovery.
     * @return a new data writer which will write parquet formatted data compressed with Snappy
     */
    public static <T> ParquetWriterFactory<T> forReflectRecord(Class<T> type) {
        String schemaString = ReflectData.AllowNull.get().getSchema(type).toString();
        ParquetBuilder<T> builder = (out) -> {
            return createAvroParquetWriter(schemaString, ReflectData.AllowNull.get(), out);
        };
        return new ParquetWriterFactory<>(builder);
    }
}
