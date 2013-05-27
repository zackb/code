package main

import (
    "grab"
    "fmt"
    "os"
    "bufio"
    "strings"
)

type GrabResult struct  {
    Url string
    StatusCode int
    Date int64
}

func handle(url string) {
    g,_ := grab.GrabUrl(url)
    //crawl.Crawl(g)
    for k, v := range g.Tag  {
        fmt.Println("Tag: " + k + " = " + v)
    }
    for k, v := range g.Headers  {
        fmt.Println("Header: " + k + " = " + v)
    }
    fmt.Println("Json: %v", g.Json)
    fmt.Println("Summary: %v", g.Summary())
}

func main() {
    //flag.Parse()
    //args := flag.Args()
    //url := args[0]
    in := bufio.NewReader(os.Stdin)
    input := ""
    for input != "." {
        input, err := in.ReadString('\n')
        if err != nil {
            fmt.Println(err)
            return
        }
        input = strings.TrimSpace(input)
        if input == "" {
            continue
        }
        go handle(input)
    }

    //grab.PrintJson(g.Json)


    //engine := xorm.Create("mysql", "root:G3tB43ck@/flix?charset=utf8")
    //err := engine.CreateTables(&GrabResult{})
    //id, err := engine.Insert(&GrabResult{Url:"foo"})
    //fmt.Printf("$d - %i\n", id, err)

    //res := GrabResult{}
    //res.Url = url
    //res.Date = time.Now().Unix()

    //Web()
}
