package finder

import (
    "grab"
)

type Finder interface {
    extract(*grab.Grab) Video
    support(*grab.Grab) bool
}

func GrabVideo(g *grab.Grab) Video {
    fs := []interface{} {
                VimeoFinder{},
                YouTubeFinder{}}

    for _, f := range fs   {
        if f.(Finder).support(g)    {
            return f.(Finder).extract(g)
        }
    }

    return Video{}
}
