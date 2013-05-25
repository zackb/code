package main

import (
    "grab"
    "flag"
    "fmt"
    "time"
)

type GrabResult struct  {
    Url string
    StatusCode int
    Date int32
}


func main() {
    flag.Parse()
    args := flag.Args()
    url := args[0]
    g := grab.GrabUrl(url)

    fmt.Printf("%s", g.Header("Content-Type"))
    for k, v := range g.Headers {
        fmt.Printf("'" + k + "' = " + v + "\n");
    }

    //grab.PrintJson(g.Json)

    s := g.Json["datetime"]

    fmt.Printf("%s\n", s)
    //Web()

    //engine := xorm.Create("mysql", "root:G3tB43ck@/flix?charset=utf8")
    //err := engine.CreateTables(&GrabResult{})
    //id, err := engine.Insert(&GrabResult{Url:"foo"})
    //fmt.Printf("$d - %i\n", id, err)

    res := GrabResult{}
    res.Url = url
    res.Date = int32(time.Now().Unix())

}
