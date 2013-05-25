package grab

import (
    "testing"
)

func Test_ParseJsonDoc(t *testing.T) {
    url := "http://api.twitter.com/1/statuses/user_timeline.json?screen_name=jack"
    g := GrabUrl(url)
    if g.Json.Get(1).Get("user").Str("screen_name") != "jack"  {
        t.Error("failed parsing tweet user screen_name")
    }
}

func Test_Vimeo(t *testing.T)   {
    url := "http://vimeo.com/api/v2/video/66753522.json"
    g := GrabUrl(url)
    if g.StatusCode != 200  {
        t.Error("status code", g.StatusCode)
    }
    if g.Json.Get(0).Int("id") != 66753522  {
        t.Error("failed parsing vimeo id")
    }
    if g.Json.Get(0).Float("id") != 66753522.0  {
        t.Error("failed parsing float vimeo id")
    }
    if g.Json.Get(0).Str("user_name") != "etnies" {
        t.Error("failed parsing string vimeo user_name")
    }
}

