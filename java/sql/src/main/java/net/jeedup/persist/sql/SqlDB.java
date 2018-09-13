package net.jeedup.persist.sql;

import net.jeedup.common.reflect.Reflect;
import net.jeedup.common.util.StringUtil;

import java.lang.reflect.Field;
import java.sql.SQLException;
import java.util.Map;
import java.util.function.Function;
import java.util.logging.Level;
import java.util.logging.Logger;

import static java.util.stream.Collectors.toMap;
import static net.jeedup.common.util.StringUtil.snake;
import static net.jeedup.common.util.Util.empty;

/**
 * Created by zack on 9/12/18.
 */
public class SqlDB<T> {

    private static final Logger log = Logger.getLogger(SqlDB.class.getSimpleName());

    private final Class<T> clazz;
    private final Sql sql;
    private final String tableName;
    private final Map<String, Field> fields;

    private final Map<String, String> fieldToRow;
    private final Map<String, String> rowToField;

    public SqlDB(Class<T> clazz, Sql sql, String tableName) {
        this.sql = sql;
        this.clazz = clazz;
        this.tableName = tableName;

        this.fields = Reflect.describeFields(clazz);

        this.fieldToRow = fields.keySet().stream()
                .collect(toMap(Function.identity(), StringUtil::snake));

        this.rowToField = fields.keySet().stream()
                .collect(toMap(StringUtil::snake, Function.identity()));
    }

    public SqlDB(Class<T> clazz, Sql sql) {
        this(clazz, sql, snake(clazz.getSimpleName()));
    }

    /**
     * Persists an object to a datastore. If the object already exists it is updated.
     * @param obj to store
     */
    public <T> void save(T obj) {
        //insertOrUpdate(obj);
    }

    /**
     * Retrieve a single object from the database whose attribute matches a value
     * @param attributeName field of object to query on
     * @param attributeValue value of field
     * @return single object or null if no attribute matches the value
     */
    public T findBy(String attributeName, Object attributeValue) throws SQLException  {
        String query = String.format("select * from %s where `%s` = %s limit 1",
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
        if (id(obj) == null) {
            // Sql().executeInsert(describeInsertOrUpdateSql(), values(obj));
        } else {
            // def res = Sql().executeInsert(describeInsertSql(), values(obj));
            // if (res) {
                // setId(obj, res[0][0])
            // }
        }
    }

    /**
     * Gets the value of the identifier field of this object.
     * For now this is hardcoded as the 'id' field.
     * @param obj to get the identifier value
     * @return value of the identifier
     */
    public <T> Object id(T obj) throws IllegalAccessException {
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
            obj = (T)clazz.newInstance();
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

            try {
                field.set(obj, value);
            } catch (IllegalAccessException e) {
                String.format(
                    "Failed setting field %s %s to %s %s",
                        field.getType(), field.getName(), value == null ? null :
                                value.getClass(), value);
            }
        }
        return obj;
    }
}
