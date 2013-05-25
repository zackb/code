package finder

import (
    "testing"
    "grab"
)

func Test_Vimeo(t *testing.T) {
    g := grab.GrabUrl("http://vimeo.com/66480704")
    v := GrabVideo(&g)
    if v.Title != "Genesis" {
        t.Error("wrong title", v.Title)
    }
}

func Test_YouTube(t *testing.T) {
    g := grab.GrabUrl("http://www.youtube.com/watch?v=f6kdp27TYZs")
    v := GrabVideo(&g)
    if v.Title != "Google I/O 2012 - Go Concurrency Patterns" {
        t.Error("wrong title", v.Title)
    }
}
