package finder

import (
    "grab"
    "regexp"
)

type YouTubeFinder struct { }

func (f YouTubeFinder)supportRegex() string {
    return `^(?:https?:\/\/)?(?:www\.)?youtube.com/`
}

func (f YouTubeFinder)supportRegexp() *regexp.Regexp {
    return regexp.MustCompile(f.supportRegex())
}

func (f YouTubeFinder)extract(g *grab.Grab) Video  {
    v := Video{}
    v.Title = g.Html.Tag["og:title"]
    return v
}

func (f YouTubeFinder)support(g *grab.Grab) bool    {
    return f.supportRegexp().MatchString(g.Url)
}
