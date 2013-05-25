package grab

import  (
    "encoding/json"
    "fmt"
)

//func ParseJson(bytes []byte) map[string]interface{} {
func ParseJson(bytes []byte) Json   {
    var f interface{}
    json.Unmarshal([]byte(bytes), &f)
    if f == nil {
        return Json{}
    }
    m := f.(map[string]interface{})
    return Json{Data:m}
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
