package grab

import (
    "testing"
)

func Test_ParseJsonDoc(t *testing.T) {
    url := "http://api.twitter.com/1/statuses/user_timeline.json?screen_name=jack"
    g,_ := GrabUrl(url)
    if g.Json.Get(1).Get("user").Str("screen_name") != "jack"  {
        t.Error("failed parsing tweet user screen_name")
    }
}

func Test_Vimeo(t *testing.T)   {
    url := "http://vimeo.com/api/v2/video/66753522.json"
    g,_ := GrabUrl(url)
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

func Test_UrlToId(t *testing.T) {
    url := "http://www.example.com/path/to/file?var=val&v2=val2"
    for i := 0; i < 100; i++ {
        id := UrlToId(url)
        if id != "com.example.www:http/path/to/file?var=val&v2=val2" {
            t.Error("invalid id: " + id)
        }

        idUrl := IdToUrl(id)
        if url != idUrl {
            t.Error("invalid id to url: " + idUrl)
        }
    }
}

func Test_SummarizeHtml(t *testing.T) {
    url := "http://blog.stewtopia.com/2013/05/30/android-users-arent-cheap-but-cheap-users-use-android/"
    g,_ := GrabUrl(url)
    summ := g.Summary()
    t.Error(summ)
}
