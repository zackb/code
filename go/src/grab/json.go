package grab

import (
    "fmt"
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

