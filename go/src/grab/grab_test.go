package grab

import (
    "testing"
)

func Test_ParseJsonDoc(t *testing.T) {
    url := "http://api.twitter.com/1/statuses/user_timeline.json?screen_name=jack"
    g := GrabUrl(url)
    arr := ParseJsonArr(g.Data)
    if arr.Get(0).Get("user").Str("screen_name") != "jack"  {
        t.Error("DD")
    }
}

