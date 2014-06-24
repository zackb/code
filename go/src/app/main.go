package main

import (
    "grab"
    "fmt"
    "os"
    "bufio"
    "strings"
    "time"
    "sync"
    //"grab/db"
    _ "github.com/go-sql-driver/mysql"
    "github.com/lunny/xorm"
    rss "github.com/frequency/go-pkg-rss"
)


type Feed struct {
    Id int64
    FeedUrl string `xorm:"feedUrl"`
}

func main() {
    engine := xorm.Create("mysql", "xxxx:xxxxx@tcp(127.0.0.1:3306)/frequency_feedfront?charset=utf8")
    var feeds []Feed
    err := engine.Table("RssFeed").Where("stateId <> ? and feedParserTypeId is null", "3").Find(&feeds) 
    if err != nil {
        fmt.Printf("Err: %v", err)
    }

    num_concurrent := 100
    urls :=  make(chan string, num_concurrent)
    var wg sync.WaitGroup
    for i := 0; i < num_concurrent; i++ {
        go func (){
            for url := range urls {
                PollFeed(url, 5)
                wg.Done()
            }
        }()
    }
    for _,feed := range feeds {
        wg.Add(1)
        urls <-feed.FeedUrl
    }
    wg.Wait()
    fmt.Println("Finished")

    select {}
}

func PollFeed(uri string, timeout int) {
    feed := rss.New(timeout, true, chanHandler, itemHandler)
    for {
        if err := feed.Fetch(uri, nil); err != nil {
            fmt.Fprintf(os.Stderr, "[e] %s: %s\n\n", uri, err)
            return
        }
        <-time.After(time.Duration(feed.SecondsTillUpdate() * 1e9))
    }
}

func chanHandler(feed *rss.Feed, newchannels []*rss.Channel) {
    fmt.Printf("%d new channel(s) in %s\n", len(newchannels), feed.Url)
}

func itemHandler(feed *rss.Feed, ch *rss.Channel, newitems []*rss.Item) {
    fmt.Printf("%d new item(s) in %s\n", len(newitems), feed.Url)
}


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

func wmain() {
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
