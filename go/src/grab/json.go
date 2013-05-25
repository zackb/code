package grab

import (
    "encoding/json"
    "fmt"
    "reflect"
)

type Json struct    {
    data map[string]interface{}
    arr []interface{}
}

func (json Json) Str(key string) string   {
    return fmt.Sprintf("%s", json.data[key])
}

func (json Json) Get(key interface{}) Json  {
    var v interface{}
    switch key.(type) {
    case string:
        v = json.data[key.(string)]
    case int:
        v = json.arr[key.(int)]
    }

    switch v.(type) {
        case []interface{}:
            return Json{arr:v.([]interface{})}
        case  map[string]interface{}:
            return Json{data:v.(map[string]interface{})}
    }
    return Json{}
}

func (json Json) Float(key interface{}) float64  {
    switch key.(type)   {
        case string:
            return json.data[key.(string)].(float64)
        case int:
            return json.arr[key.(int)].(float64)
    }

    return 0.0
}

func (json Json) Int(key interface{}) int  {
    x := json.Float(key)
    return int(x)
}

func ParseJson(bytes []byte) Json   {
    var f interface{}
    json.Unmarshal([]byte(bytes), &f)
    if f == nil {
        return Json{}
    }
    switch f.(type)   {
        case []interface{}:
            return Json{arr:f.([]interface{})}
        case  map[string]interface{}:
            m := f.(map[string]interface{})
            return Json{data:m}
        default:
            fmt.Println("Dont Know: %i", reflect.TypeOf(f))
    }
    return Json{}
}

func PrintJson(json map[string]interface{})   {
    for k, v := range json {
        switch vv := v.(type) {
        case string:
            fmt.Println(k, "is string", vv)
        case int:
            fmt.Println(k, "is int", vv)
        case []interface{}:
            fmt.Println(k, "is an array:")
            for i, u := range vv {
                fmt.Println(i, u)
            }
        default:
            fmt.Println(k, "is of a type I don't know how to handle")
        }
    }
}
