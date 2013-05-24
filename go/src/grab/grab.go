package grab

import (
    "github.com/opesun/goquery"
    "github.com/opesun/goquery/exp/html"
    "log"
    "net/http"
    "io/ioutil"
    "reflect"
    "strings"
)

type Grab struct    {
    Tags map[string]string 
    Etag string
}

func GrabUrl(url string) Grab  {
    grab := Grab{}
    grab.Tags = GrabTags(url)
    return grab
}

func GrabTags(url string) map[string]string   {

    tags := []string{"title", "description"} 
    metas := []string{"og:", "twitter:"} 

    data := map[string]string{} 

    html := download(url) 

    nodes := parse(html)

    head := nodes.Find("head")

    for _, node := range head.Find("*")    {
        //log.Println(node.Type) //log.Println(node.Data) //log.Println(node.Attr)

        for _, tag := range tags  {
            switch node.Data    {
                case tag:
                    data[tag] = node.Child[0].Data
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
                            data[name] = value
                        }
                    }
            }
        }
    }

    return data
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

func download(url string) string    {
    resp, err := http.Get(url)
    if err != nil {
        log.Println("request failed - %s %s", url, err.Error())
    }

    defer resp.Body.Close()

    data, err := ioutil.ReadAll(resp.Body)
    if err != nil {
        log.Println("read failed - %s %s", url, err.Error())
    }
    return string(data)
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
