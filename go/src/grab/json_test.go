package grab

import (
    "testing"
)

func Test_ParseJsonArr(t *testing.T)    {
    s := `[{"i":4}]`
    json := ParseJson([]byte(s))
    if json.Get(0).Int("i") != 4   {
        t.Error("arr int parse failed")
    }
}

func Test_ParseJson(t *testing.T) {
    s := `{"o": 1, "s": "mystr", "m":{"foo":"bar", "i": 100}, "a":[4, {"mm":"mv"}, 11, {"aa":[54]}]}`
    json := ParseJson([]byte(s))
    if json.Int("o") != 1   {
        t.Error("int parse failed")
    }

    if json.Float("o") != 1.0  {
        t.Error("float parse failed")
    }

    if json.Str("s") != "mystr"  {
        t.Error("str parse failed")
    }

    if json.Get("m").Str("foo") != "bar"  {
        t.Error("str parse failed")
    }

    if json.Get("m").Int("i") != 100    {
        t.Error("map int parse failed")
    }

    if json.Get("a").Int(0) != 4    {
        t.Error("array int parse failed")
    }

    if json.Get("a").Get(1).Str("mm") != "mv"    {
        t.Error("array map parse failed")
    }

    if json.Get("a").Float(2) != 11.0    {
        t.Error("array float parse failed")
    }

    if json.Get("a").Get(3).Get("aa").Int(0) != 54  {
        t.Error("array aray int parse failed")
    }
}

