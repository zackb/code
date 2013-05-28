package finder

import (
    "grab"
    "regexp"
)

type VimeoFinder struct { }


func (f VimeoFinder)extract(g *grab.Grab) Video  {
    v := Video{}
    v.Title = g.Html.Meta["og:title"]
    return v
}

func (f VimeoFinder)supportRegex() string {
    return `^(?:http:\/\/)?(?:www.|player.)?vimeo.com`
}

func (f VimeoFinder)supportRegexp() *regexp.Regexp {
    return regexp.MustCompile(f.supportRegex())
}

func (f VimeoFinder)support(g *grab.Grab) bool    {
    return f.supportRegexp().MatchString(g.Url)
}
