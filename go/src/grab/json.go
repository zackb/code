package grab

import (
    "encoding/json"
    "fmt"
    "reflect"
)

type Json struct    {
    Data map[string]interface{}
    arr []interface{}
}

type JsonArr struct {
    Data []interface{}
}

func (json Json) Str(key string) string   {
    return fmt.Sprintf("%s", json.Data[key])
}

func (json Json) Get(key string) Json  {
    j := Json{Data:json.Data[key].(map[string]interface{})}
    return j
}

func (json Json) Float(key string) float64  {
    return json.Data[key].(float64)
}

func (json Json) Int(key string) int  {
    x := json.Float(key)
    return int(x)
}

func (json Json) Arr(key string) JsonArr    {
    d := json.Data[key].([]interface{})
    return JsonArr{Data:d}
}

func (arr JsonArr) Str(i int) string {
    return fmt.Sprintf("%s", arr.Data[i])
}

func (arr JsonArr) Float(i int) float64 {
    return arr.Data[i].(float64)
}

func (arr JsonArr) Int(i int) int {
    return int(arr.Float(i))
}

func (arr JsonArr) Get(i int) Json  {
    j := Json{Data:arr.Data[i].(map[string]interface{})}
    return j
}

func ParseJson(bytes []byte) Json   {
    var f interface{}
    json.Unmarshal([]byte(bytes), &f)
    if f == nil {
        return Json{}
    }
    switch f.(type)   {
        case []interface{}:
            //fmt.Println("Dont Know: %i", reflect.TypeOf(f))
            //aa := f.([]map[string]interface{})
            //a := make([]map[string]interface{}, aa[0].(map[string]interface{}))
            //a[0] = f.(map[string]interface{})
            //a := f.([]map[string]interface{})
            /*
            m := make(map[string]interface{})
            for k, v := range vf {
                m[k] = v
            }
            */
            //return Json{Data:m}
        case  map[string]interface{}:
            m := f.(map[string]interface{})
            return Json{Data:m}
        default:
            fmt.Println("Dont Know: %i", reflect.TypeOf(f))
    }
    return Json{}
}

func ParseJsonArr(bytes []byte) JsonArr {
    var f []interface{}
    json.Unmarshal([]byte(bytes), &f)
    if f == nil {
        return JsonArr{}
    }
    return JsonArr{Data:f}
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
