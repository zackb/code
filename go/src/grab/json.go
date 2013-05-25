package grab

import (
    "fmt"
)

type Json struct    {
    Data map[string]interface{}
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
