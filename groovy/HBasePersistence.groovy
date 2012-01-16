package com.frequency.hbase

import org.apache.commons.logging.Log
import org.apache.commons.logging.LogFactory
import org.apache.hadoop.hbase.client.HBaseAdmin
import org.apache.hadoop.conf.Configuration
import org.apache.hadoop.hbase.client.HTablePool
import org.apache.hadoop.hbase.HBaseConfiguration
import org.apache.hadoop.hbase.util.Bytes
import org.apache.hadoop.hbase.client.Put
import java.lang.reflect.Field
import java.lang.reflect.Modifier
import org.apache.hadoop.hbase.client.HTable
import org.apache.hadoop.hbase.HColumnDescriptor
import org.apache.hadoop.hbase.HTableDescriptor
import org.apache.hadoop.hbase.client.Get
import org.apache.hadoop.hbase.client.Result

/**
 * User: zack
 * Date: Jan 14, 2012
 */
class HBasePersistence<T>   {

    private static Log LOG = LogFactory.getLog(HBasePersistence.class)

    private static Configuration config

    private static HBaseAdmin admin

    private static HTablePool tablePool = new HTablePool(getHbaseConfiguration(), 100)

    private Class clazz

    private Object delegate

    private String tableName

    private static final String PROPERTY_COLUMN_FAMILY = "props"

    private static final byte[] PROPERTY_COLUMN_FAMILY_BYTES = bytes(PROPERTY_COLUMN_FAMILY)

    private static List<Class> PERSISTABLE_CLASSES = [
        String.class,
        Long.class,
        Integer.class,
        Double.class,
        Short.class,
        Date.class,
        String[].class
    ]

    public HBasePersistence(Class clazz, Object delegate = null)   {
        this.clazz = clazz
        this.delegate = delegate
        this.tableName = clazz.name
    }
    
    public Object getId()   {
        return delegate.id
    }

    public void save(Long version = null)  {
        withTable({ HTable table ->
            Put put = new Put(Bytes.toBytes(getId().toString()), version)
            for (Field field : persistibleFields()) {
                field.accessible = true
                Object value = field.get(this.delegate)
                if (value != null)  {
                    persist(put, PROPERTY_COLUMN_FAMILY, field.name, value)
                }
            }
            table.put(put)
        })
    }

    public T get(String id)  {
        HTable table = getTable()
        try {
            Get g = new Get(bytes(getId()))
            Result r = table.get(g)
            def result = null //deserialize(clazz, r);
            return result
        } finally {
            returnTable(table);
        }
    }

    public void ensureTable(boolean drop = false)   {
        HBaseAdmin a = getHbaseAdmin()
        HTableDescriptor table = null
        if (drop) {
            deleteTable()
        }
        boolean exists = a.tableExists(tableName)
        if (!exists) {
            LOG.info("Creating HBase table named ${tableName}")
            table = new HTableDescriptor(tableName)
            a.createTable(table)
        }

        ensureColumnFamily(PROPERTY_COLUMN_FAMILY)
    }

    public void deleteTable()    {
        HBaseAdmin a = getHbaseAdmin()
        if (a.tableExists(tableName)) {
            LOG.info("Deleting HBase table ${tableName}")
            a.disableTable(tableName)
            a.deleteTable(tableName)
        }
    }

    public static void addSupport(Class clazz)  {
        clazz.metaClass.getHbase = { ->
            return new HBasePersistence(clazz, delegate)
        }

        clazz.metaClass.static.getHbase = { ->
            return new HBasePersistence(clazz)
        }
    }

    public static Configuration getHbaseConfiguration()  {
        if (!config)  {
            //config = com.frequency.utils.Env.getBean("hbaseServer").getHbaseConfiguration()
            config = HBaseConfiguration.create()
            config.set("hbase.zookeeper.quorum", "jax")
        }

        return config;
    }

    public static HBaseAdmin getHbaseAdmin()  {
        if (!admin) {
            admin = new HBaseAdmin(getHbaseConfiguration())
        }

        return admin
    }

    private void persist(Put put, String columnFamily, String name, Object value)  {
        put.add(bytes(columnFamily), bytes(name), bytes(value))
    }
    
    private boolean persistible(Class c)    {
        return PERSISTABLE_CLASSES.contains(c)
    }

    private boolean persistible(Field field)    {
        if (field == null)  {
            return false;
        }

        if (Modifier.isStatic(field.modifiers))   {
            return false
        }

        if (Modifier.isAbstract(field.modifiers))   {
            return false
        }

        // groovy added stuff
        if (field.name.startsWith('$') || field.name.startsWith('metaClass'))   {
            return false
        }

        return true
    }

    private Field[] fieldCache
    private Field[] persistibleFields() {
        if (!fieldCache)    {
            List<Field> fields = []
            clazz.declaredFields.each { Field field ->
                if (persistible(field)) {
                    fields << field
                }
            }
            fieldCache = fields as Field[]
        }

        return fieldCache
    }

    private HTable getTable()  {
        HTable result = (HTable)tablePool.getTable(tableName)
        return result
    }

    private void returnTable(HTable table) {
        tablePool.putTable(table)
    }

    public void withTable(Closure c)  {
        HTable table = getTable()
        try {
            c.call(table)
        } finally {
            returnTable(table)
        }
    }

    private void ensureColumnFamily(String columnFamily)    {
        HBaseAdmin a = getHbaseAdmin()
        HTableDescriptor table = a.getTableDescriptor(bytes(tableName))
        byte[] columnFamilyBytes = bytes(columnFamily)
        if (!table.hasFamily(columnFamilyBytes))  {
            a.disableTable(tableName)
            HColumnDescriptor column = new HColumnDescriptor(columnFamilyBytes)
            a.addColumn(tableName, column)
            a.enableTable(tableName)
        }
    }

    public void hello() {
        println "Hello"
    }

    private Object fromBytes(byte[] bytes, Class type)  {
        assert persistible(type) : "Not supported type: ${type}"
        Object value = null
        switch (type)   {
            case String:
                value = Bytes.toString(bytes)
                break
            case Date:
                value = new Date(Bytes.toLong(bytes))
                break
            case Long:
                value = Bytes.toLong(bytes)
                break
            case Double:
                value = Bytes.toDouble(bytes)
                break
            case Integer:
                value = Bytes.toInt(bytes)
                break
            case Short:
                value = Bytes.toShort(bytes)
                break
            case Object[]:
                ByteArrayInputStream bin = new ByteArrayInputStream(bytes)
                ObjectInputStream oin = new ObjectInputStream(bin)
                value = oin.readObject()
                break
            default:
                throw new Exception("Unsupported type List")
                break
        }

        return value
    }


    private static byte[] bytes(String str) {
        return Bytes.toBytes(str)
    }

    private static byte[] bytes(Date date) {
        return Bytes.toBytes(date.time)
    }

    private static byte[] bytes(Long l)    {
        return Bytes.toBytes(l)
    }

    private static byte[] bytes(Double d)    {
        return Bytes.toBytes(d)
    }

    private static byte[] bytes(Integer i)    {
        return Bytes.toBytes(i)
    }

    private static byte[] bytes(Short s)    {
        return Bytes.toBytes(s)
    }

    private static byte[] bytes(List<?> list)    {
        // TODO: do this better
        String value = list.join(",")
        return Bytes.toBytes(value)
    }

    private static byte[] bytes(Object[] array) {
          ByteArrayOutputStream baos = new ByteArrayOutputStream()
          ObjectOutputStream os = new ObjectOutputStream(baos)
          os.writeObject(array)
          return baos.toByteArray()
    }

    /*
    private static byte[] bytes(Object obj) {
        if (obj == null)    {
            return null
        }
        assert false : "Not supported type: ${obj.class}"
        return null
    }
    */
}
