package grab

import (
    "github.com/opesun/goquery"
    "github.com/opesun/goquery/exp/html"
    "log"
    "net/http"
    "io/ioutil"
    "strings"
    "reflect"
    "grab/text"
)

type Grab struct    {
    Url string
    Tag map[string]string
    Data []byte
    Html string
    Json Json
    StatusCode int
    Headers map[string]string
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
            grab.Html = string(grab.Data)
            GrabTags(grab)
    }
    return grab,nil
}

func (g Grab)Summary() string {
    return text.Summarize(g.Tag["title"], g.Html)
}

func GrabTags(g *Grab) {

    tags := []string{"title", "description"}
    metas := []string{"og:", "twitter:", "description"}

    g.Tag = map[string]string{}

    nodes := parse(g.Html)

    head := nodes.Find("head")

    for _, node := range head.Find("*")    {
        //log.Println(node.Type) //log.Println(node.Data) //log.Println(node.Attr)

        for _, tag := range tags  {
            switch node.Data    {
                case tag:
                    g.Tag[tag] = node.Child[0].Data
                case "meta":
                    attrs := attr_map(node.Attr)
                    name := attrs["property"]

                    if name == ""   {
                        name = attrs["name"]
                    }

                    for _, meta := range metas    {
                        if strings.HasPrefix(name, meta)   {
                            value := attrs["value"]
                            if value == ""  {
                                value = attrs["content"]
                            }
                            g.Tag[name] = value
                        }
                    }
            }
        }
    }
}

func attr_map(attrs []html.Attribute) map[string]string  {
    data := map[string]string{}
    for _, attr := range attrs  {
        data[attr.Key] = attr.Val
    }
    return data
}


func parse(html string) goquery.Nodes    {
    nodes, err := goquery.Parse(html)
    if err != nil {
        log.Println("parsing failed - %s %s", err.Error())
    }
    return nodes
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
