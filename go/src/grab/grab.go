package grab

import (
    "github.com/PuerkitoBio/goquery"
    "code.google.com/p/go.net/html"
    "log"
    "net/http"
    "io/ioutil"
    "strings"
    "reflect"
    "bytes"
    "grab/text"
)

type Grab struct    {
    Url string
    Data []byte
    Html Html
    Json Json
    StatusCode int
    Headers map[string]string
}

type Html struct {
    Data string
    Meta map[string]string
    Body string
    Tags []string
}

func (g *Grab) Header(key string) string {
    return g.Headers[strings.ToLower(key)]
}

func GrabUrl(url string) (*Grab, error)  {
    grab, err := download(url)
    if err != nil   {
        log.Println("download failed - ", err.Error())
        return nil, err
    }
    contentType := grab.Header("content-type")
    switch  {
        case strings.Contains(contentType, "application/json"):
            grab.Json = ParseJson(grab.Data)
        case contentType == "" || strings.Contains(contentType, "text/html"):
            grab.Html = Html{}
            grab.Html.Data = string(grab.Data)
            GrabMeta(grab)
    }
    return grab,nil
}

func (g Grab) Item() *Item {
    item := Item{id:1}
    item.Title = first(g.Html.Meta["og:title"], g.Html.Meta["title"])
    item.Description = first(g.Html.Meta["og:description"], g.Html.Meta["description"])
    item.Url = g.Url
    item.Tags = g.Html.Tags
    return &item
}

func (g Grab)Summary() string {
    return text.Summarize(g.Html.Meta["title"], g.Html.Body)
}

func GrabMeta(g *Grab) error {

    tags := map[string]bool {"title" : true, "description" : true}
    //metas := []string{"og:", "twitter:", "description", "keywords"}

    g.Html.Meta = map[string]string{}

    nodes, err := parseHtml(g.Html.Data)
    if err != nil {
        return err
    }

    head := nodes.Find("head")

    head.Find("*").Each(func(i int, s *goquery.Selection) {
        for _,node := range s.Nodes  {
            switch node.Type    {
            case html.ErrorNode:
            case html.TextNode:
            case html.DocumentNode:
            case html.ElementNode:
                if tags[node.Data] {
                    g.Html.Meta[node.Data] = node.FirstChild.Data
                }
                switch node.Data {
                case "meta":
                    var k, v string
                    for _,attr := range node.Attr    {
                        if attr.Key == "property" || attr.Key == "name" {
                            k = attr.Val
                        } else if attr.Key == "content" || attr.Key == "value" {
                            v = attr.Val
                        }
                    }
                    if k != "" && v != "" {
                        g.Html.Meta[k] = v
                    }
                }
            case html.CommentNode:
            case html.DoctypeNode:
            }
        }
    })

    nodes.Find("body").Each(func(i int, s *goquery.Selection) {
        for _,node := range s.Nodes  {
            if node.Data == "body"  {
                g.Html.Body = getNodeText(node)
                break
            }
        }
    })

    for _,s := range strings.Split(g.Html.Meta["keywords"], ",") {
        g.Html.Tags = append(g.Html.Tags, strings.TrimSpace(s))
    }

    return nil
}

func parseHtml(h string) (*goquery.Document, error) {
    node, err := html.Parse(strings.NewReader(h))
    if err != nil {
        log.Println("parsing failed - %s %s", err.Error())
        return nil, err
    }
    doc := goquery.NewDocumentFromNode(node)
    return doc,nil
}

func download(url string) (*Grab,error)  {
    grab := Grab{Url:url}
    resp, err := http.Get(url)
    if err != nil {
        log.Println("request failed - %s %s", url, err.Error())
        return nil,err
    }

    defer resp.Body.Close()

    data, err := ioutil.ReadAll(resp.Body)
    if err != nil {
        log.Println("read failed - %s %s", url, err.Error())
        return nil,err
    }

    grab.Data = data
    grab.StatusCode = resp.StatusCode

    headers := resp.Header
    grab.Headers = make(map[string]string)

    for k, v := range headers   {
        grab.Headers[strings.ToLower(k)] = strings.Join(v, ",")
    }

    return &grab,nil
}

func getNodeText(node *html.Node) string {
    if node.Data == "script"  {
        return ""
    }
    if node.Type == html.TextNode {
        text := strings.TrimSpace(node.Data)
        if text == "" {
            return ""
        }
        return text + "\n"
    } else if node.FirstChild != nil {
        var buf bytes.Buffer
        for c := node.FirstChild; c != nil; c = c.NextSibling {
            buf.WriteString(getNodeText(c))
        }
        return buf.String()
    }

    return ""
}

func first(obs ...string) string {
    for _,o := range obs {
        if o != "" {
            return o
        }
    }
    return ""
}

func dump(v interface{})  {
    t := reflect.TypeOf(v)
    log.Printf("Type: %s", t.Kind())
    for i := 0; i < t.NumMethod(); i++ {
        m := t.Method(i)
        log.Printf("Method(%d): %s %s = %v\n", i, m.Name, m.Type)
    }

    for i := 0; i < t.NumField(); i++ {
        f := t.Field(i)
        log.Printf("Method(%d): %s %s = %v\n", i, f.Name, f.Type)
    }
}
