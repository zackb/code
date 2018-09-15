package net.jeedup.sql;

import net.jeedup.common.reflect.Reflect;
import net.jeedup.common.util.StringUtil;

import java.lang.reflect.Field;
import java.math.BigDecimal;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.function.Function;
import java.util.logging.Level;
import java.util.logging.Logger;

import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toMap;
import static net.jeedup.common.util.StringUtil.snake;
import static net.jeedup.common.util.Util.empty;

/**
 * http://jdbi.org/ is probably better
 * Created by zack on 9/12/18.
 */
public class SqlDB<T> {

    private static final Logger log = Logger.getLogger(SqlDB.class.getSimpleName());

    private final Class<T> clazz;
    private final Sql sql;
    private final String tableName;
    private final Map<String, Field> fields;

    private final Map<String, String> fieldToRow;
    //private final Map<String, String> rowToField;
    private final List<String> sortedFields;

    public SqlDB(Class<T> clazz, Sql sql, String tableName) {
        this.sql = sql;
        this.clazz = clazz;
        this.tableName = tableName;

        this.fields = Reflect.describeFields(clazz);

        this.fieldToRow = fields.keySet().stream()
                .collect(toMap(Function.identity(), StringUtil::snake));

        /*
        this.rowToField = fields.keySet().stream()
                .collect(toMap(StringUtil::snake, Function.identity()));
        */

        this.sortedFields = fields.keySet().stream()
                .sorted().collect(toList());
    }

    public SqlDB(Class<T> clazz, Sql sql) {
        this(clazz, sql, snake(clazz.getSimpleName()));
    }

    /**
     * Persists an object to a datastore. If the object already exists it is updated.
     * @param obj to store
     */
    public void save(T obj) throws Exception {
        insertOrUpdate(obj);
    }

    /**
     * Inserts a row to the database. If a row with the same identifier already
     * exists an exeption is thrown.
     * @param obj to persist
     */
    public void insert(T obj) throws Exception {
        Object val = sql.executeInsert(describeInsertSql(), values(obj));
        setId(obj, val);
    }

    /**
     * Update a row in the database with the object's identifier and values.
     * @param obj to update
     */
    public void update(T obj) throws Exception {
        List<Object> values = values(obj);
        values.add(id(obj));
        sql.execute(describeUpdateSql(), values);
    }

    /**
     * Retrieves and deserializes an object from a datastore given an identifier.
     * @param id primary identifier for an object
     * @return deserialized object for the given identifier
     */
    public T get(Object id) throws SQLException {
        return findBy("id", id);
    }

    /**
     * Retrieve a single object from the database whose attribute matches a value
     * @param attributeName field of object to query on
     * @param attributeValue value of field
     * @return single object or null if no attribute matches the value
     */
    public T findBy(String attributeName, Object attributeValue) throws SQLException  {
        String query = String.format("select * from %s where `%s` = '%s' limit 1",
                tableName, attributeName, attributeValue);

        Sql.Row row = sql.firstRow(query);

        return instantiate(row);
    }

    /**
     * Inserts a row to the database or updates its values if it alread exists.
     * Uses SQL 'on duplicate key update' syntax
     * @param obj to persist
     */
    public void insertOrUpdate(T obj) throws Exception {
        sql.execute(describeInsertOrUpdateSql(), values(obj));
        /*
        if (id(obj) == null) {
        } else {
            sql.execute(describeInsertSql(), values(obj));
        }
        */
    }

    /**
     * Gets the value of the identifier field of this object.
     * For now this is hardcoded as the 'id' field.
     * @param obj to get the identifier value
     * @return value of the identifier
     */
    public Object id(T obj) throws IllegalAccessException {
        Field field = fields.get("id");
        return field.get(obj);
    }

    /**
     * Creates an object of type T and populates its fields with given values
     * @return newly created empty object
     */
    public T instantiate() {
        return instantiate(null);
    }

    /**
     * Creates an object of type T and populates its fields with given values
     * @param values map of field name to values
     * @return created object populated with values
     */
    public T instantiate(Map<String, Object> values) {
        T obj = null;
        try {
            obj = clazz.newInstance();
        } catch (Exception e) {
            log.log(Level.SEVERE, "Failed instantiating object: ", e);
        }
        return merge(obj, values);
    }

    private T merge(T obj, Map<String, Object> values) {

        if (empty(values) || obj == null) {
            return obj;
        }
        // Strange compile static bug: fields.each { String name, Field field ->
        for (String name : fields.keySet()) {
            Field field = fields.get(name);
            String row = fieldToRow.get(name);
            if (!values.containsKey(row)) {
                continue;
            }
            Object value = values.get(row);
            if (field.getType() == Boolean.class &&
                    !(value instanceof Boolean)) {

                if (value instanceof String) {
                    value = !empty((String) value);
                } else if (value instanceof Long) {
                    value = !empty((Long) value);
                } else {
                    // TODO
                    //value = value.asBoolean()
                }
            }

            if (value != null && field.getType() != value.getClass()) {
                // TODO!
            }

            setEx(obj, field, value);
        }
        return obj;
    }

    private static void setEx(Object obj, Field field, Object value) {
        try {
            field.set(obj, coerce(field, value));
        } catch (IllegalAccessException e) {
            log.log(Level.SEVERE, String.format(
                    "Failed setting field %s %s to %s %s",
                    field.getType(), field.getName(), value == null ? null :
                            value.getClass(), value), e);
        }
    }

    static Object coerce(Field field, Object value) {

        if (value == null)
            return value;

        Object result = value;

        Class<?> fieldType = field.getType();
        Class<?> valueType = value.getClass();

        if (fieldType != valueType &&
            !fieldType.isAssignableFrom(valueType)) {

            if (value instanceof BigDecimal) {
                if (fieldType == Double.class || fieldType == double.class) {
                    result = ((BigDecimal)value).doubleValue();
                }
            }
        }

        return result;
    }

    private static Object getEx(Object obj, Field field) {
        Object val = null;
        try {
            val = field.get(obj);
        } catch (IllegalAccessException e) {
            log.log(Level.SEVERE, String.format(
                    "Failed getting field %s %s from %s ",  field.getType(), field.getName(), obj.getClass()), e);
        }
        return val;
    }

    /**
     * Get field values of an object.
     * @param obj object to get values for
     * @return list of values of the object's fields
     */
    private List<Object> values(T obj) {

        List<Object> values = new ArrayList<>();

        if (obj == null)
            return values;

        sortedFields.forEach(name -> {
            Field field = fields.get(name);
            Object value = getEx(obj, field);
            if ("updated".equals(name)) {
                values.add(new Date());
            } else if ("created".equals(name) && value == null) {
                values.add(new Date());
            } else {
                values.add(value);
            }
        });

        return values;
    }

    private static Map<String, String> insertOrUpdateSqlCache = new ConcurrentHashMap<>();

    /**
     * Generates a SQL insert and 'on duplicate key update' statement for this object.
     * The result is cached so is only computed once.
     * @return SQL insert or update statement
     */
    protected final String describeInsertOrUpdateSql() {

        String inserOrUpdateSql = insertOrUpdateSqlCache.get(tableName);

        if (!empty(inserOrUpdateSql)) {
            return inserOrUpdateSql;
        }

        StringBuilder updateSql = new StringBuilder();
        StringBuilder insertSql = new StringBuilder();

        //insertSql.append("insert into `").append(tableName).append("` (");
        insertSql.append("insert into ").append(tableName).append(" (");

        int i = 0;
        for (String name : sortedFields) {
            //insertSql.append("`").append(name).append("`");
            insertSql.append(name);

            updateSql.append("`").append(name).append("` = coalesce(values(`").append(name).append("`), `").append(name).append("`)");
            if (++i < fieldToRow.size()) {
                insertSql.append(",");
                updateSql.append(",");
            }
        }

        insertSql.append(") values (").append(questionMarksWithCommas(sortedFields.size())).append(")");

        insertSql.append(" on duplicate key update ").append(updateSql);

        String isql = insertSql.toString();
        insertOrUpdateSqlCache.put(tableName, isql);

        return isql;
    }

    private static Map<String, String> insertSqlCache = new ConcurrentHashMap<>();

    /**
     * Generates a SQL insert statement for this object.
     * The result is cached so is only computed once.
     * @return SQL insert statement
     */
    public String describeInsertSql() {
        String isql = insertSqlCache.get(tableName);
        if (!empty(isql)) {
            return isql;
        }

        StringBuilder insertSql = new StringBuilder("insert into " + tableName + " (");

        for (String name : sortedFields) {
            insertSql.append("`").append(name).append("`,");
        }

        // trim last ,
        insertSql = new StringBuilder(insertSql.substring(0, insertSql.length() - 1));

        insertSql.append(") values (").append(questionMarksWithCommas(sortedFields.size())).append(" ) ");

        isql = insertSql.toString();

        insertSqlCache.put(tableName, isql);

        return isql;
    }

    private static Map<String, String> updateSqlCache = new ConcurrentHashMap<>();

    /**
     * Generates a SQL update statement for this object.
     * The result is cached so is only computed once.
     * @return SQL update statement
     */
    private final String describeUpdateSql() {

        String usql = updateSqlCache.get(tableName);

        if (!empty(usql)) {
            return usql;
        }

        StringBuilder updateSql = new StringBuilder("update `").append(tableName).append("` set ");

        for (String name : sortedFields) {
            updateSql.append("`").append(name).append("` = ?,");
        }

        // trim last ,
        updateSql = new StringBuilder(updateSql.substring(0, updateSql.length() - 1));

        updateSql.append(" where `id` = ? ");

        usql = updateSql.toString();

        updateSqlCache.put(tableName, usql);

        return usql;
    }

    /**
     * Very rudimentary create table syntax, mysql only
     *
     */
    public void createTable() throws Exception {
        createTable("innodb");
    }

    public void createTable(final String engine) throws Exception {

        StringBuilder createSql = new StringBuilder();

        createSql.append("create table `").append(tableName).append("` (");

        final List<String> constraints = new ArrayList<>();

        if (fields.containsKey("id")) {

            final String idType = fields.get("id").getType().getSimpleName();

            if ("Long".equals(idType)    || "long".equals(idType) ||
                "Integer".equals(idType) || "int".equals(idType)) {
                // remove ` because h2...
                createSql.append("id int(10) unsigned not null auto_increment,");
            } else if ("String".equals(idType)) {
                createSql.append("id varchar(255) not null,"); // character set utf8
            }

            constraints.add("constraint pk_" + tableName + " primary key(id),");
        }

        for (String name : fields.keySet()) {

            if (name.equals("id"))
                continue;

            Field field = fields.get(name);

            final Constraints options = field.getAnnotation(Constraints.class);

            String datatype = "";
            boolean unique = false;
            boolean index = false;
            int numberMax = 10;
            int stringMax = 255;
            if (options != null) {
                if (options.max() > 0) {
                    numberMax = options.max();
                    stringMax = options.max();
                }
                unique = options.unique();
                index = options.index();
            }

            String type = field.getType().getSimpleName();
            switch (type) {
                case "String":
                    datatype = "varchar(" + stringMax + ")"; // character set utf8";
                    break;
                case "Long":
                case "long":
                    datatype = "bigint(" + numberMax + ")";
                    break;
                case "Integer":
                case "int":
                case "short":
                case "Short":
                    datatype = "int(" + numberMax + ")";
                    break;
                case "Double":
                case "double":
                case "Float":
                case "float":
                    datatype = "double"; // numeric?
                    break;
                case "Date":
                case "Timestamp":
                    datatype = "datetime";
                    break;
                case "byte[]": // mer?
                    datatype = "blob";
                    break;
                default:
                    throw new Exception("Do not know how to handle: ${field.type.name}");
            }

            // remove ` because of h2
            createSql.append(name).append(" ").append(datatype).append(",");

            if (unique){
                constraints.add(" constraint u_" + tableName + "_on_" + name + " unique(" + name + "),");
            }
            if (index){
                constraints.add(" index idx_" + tableName + "_on_" + name + " (" + name + "),");
            }
        }

        createSql.append(String.join(" ", constraints));

        // trim last ,
        createSql = new StringBuilder(createSql.substring(0, createSql.length() - 1));

        createSql.append(") engine=").append(engine).append(" default charset=utf8");

        sql.execute(createSql.toString());
    }

    /**
     * Sets the value of the identifier field for a given object.
     * @param obj to set identifier value
     * @param value to set identifier to
     */
    protected void setId(T obj, Object value) {
        setEx(obj, fields.get("id"), value);
    }

    private static String questionMarksWithCommas(int num) {
        if (num == 0) {
            return "";
        }

        StringBuilder s = new StringBuilder();
        for (int h = 0; h < num; h++) {
            s.append("?");
            if (h < num - 1) {
                s.append(",");
            }
        }
        return s.toString();
    }
}
