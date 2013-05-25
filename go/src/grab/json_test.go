package grab

import (
    "testing"
)

func Test_ParseJson(t *testing.T) {
    s := `{"o": 1, "s": "mystr", "m":{"foo":"bar", "i": 100}}`
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
}

