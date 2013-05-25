package main

import (
    "grab"
    "flag"
    "time"
    "fmt"
)

type GrabResult struct  {
    Url string
    StatusCode int
    Date int64
}


func main() {
    flag.Parse()
    args := flag.Args()
    url := args[0]
    g := grab.GrabUrl(url)

    for k, v := range g.Tag  {
        fmt.Println("Tag: " + k + " = " + v)
    }

    for k, v := range g.Headers  {
        fmt.Println("Header: " + k + " = " + v)
    }

    //grab.PrintJson(g.Json)


    //engine := xorm.Create("mysql", "root:G3tB43ck@/flix?charset=utf8")
    //err := engine.CreateTables(&GrabResult{})
    //id, err := engine.Insert(&GrabResult{Url:"foo"})
    //fmt.Printf("$d - %i\n", id, err)

    res := GrabResult{}
    res.Url = url
    res.Date = time.Now().Unix()

    //Web()
}
