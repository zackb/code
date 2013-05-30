package main

import (
    "grab"
    "fmt"
    "os"
    "bufio"
    "strings"
    //"grab/db"
)

type GrabResult struct  {
    Url string
    StatusCode int
    Date int64
}

func handle(url string) {
    g,err := grab.GrabUrl(url)
    if err != nil {
        fmt.Println("Error: ", err)
        return
    }
    //crawl.Crawl(g)
    for k, v := range g.Html.Meta  {
        fmt.Println("Meta: " + k + " = " + v)
    }
    for k, v := range g.Headers  {
        fmt.Println("Header: " + k + " = " + v)
    }
    fmt.Printf("Json: %v\n", g.Json)
    fmt.Printf("Summary: %v\n", g.Summary())
    //fmt.Printf("Paragraphs: %d %v\n", len(g.Html.Paragraphs), g.Html.Paragraphs)
}

func main() {
    //flag.Parse()
    //args := flag.Args()
    //url := args[0]

    //db.G()

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


    //err := engine.CreateTables(&GrabResult{})
    //id, err := engine.Insert(&GrabResult{Url:"foo"})
    //fmt.Printf("$d - %i\n", id, err)

    //res := GrabResult{}
    //res.Url = url
    //res.Date = time.Now().Unix()

    //Web()
}
