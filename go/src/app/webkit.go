package main

import (
    "grab/webkit"
	"github.com/mattn/go-gtk/gtk"
    "time"
    "fmt"
    "flag"
    "log"
    "net/http"
)
const HTML_STRING = `
<doctype html>
<meta charset="utf-8"/>
<script src="http://code.jquery.com/jquery-latest.js"></script>
<script>
$(function() {
    $('#hello1').text('DDDDDDDDDDDDD');
})
</script>
<div id="hello1" style="display: none">Hello</div>
<div id="hello2" style="display: none">世界</div>
</div>
`
const HTML_STRING_2 = `
<html>
<head>
</head>
<body>
    <div id="foo">Hello</div>
</body>
</html>
`

var addr = flag.String("addr", ":1718", "http service address") // Q=17, R=18

func main() {
	gtk.Init(nil)
    webview := webkit.NewWebView()
	webview.Connect("load-error", func() {
        //fmt.Printf("Load Error: %s\n", webview.GetUri())
	})
	webview.Connect("onload-event", func() {
        //fmt.Printf("Onload Event: %s\n", webview.GetUri())
	})
	webview.Connect("resource-load-finished", func(wv interface{}) {
        //fmt.Printf("Resource Load Finished: %v\n", wv)
	})
	webview.Connect("load-committed", func() {
		//entry.SetText(webview.GetUri())
        //fmt.Printf("Load Committed: %s\n", webview.GetUri())
	})
	webview.Connect("load-finished", func() {
        fmt.Printf("Load Finished: %s\n", webview.GetUri())
        time.Sleep(3 * time.Second)
        fmt.Printf("%s\n", getHtml(webview))
    })
    go gtk.Main()
    doit(webview)
}

func doit(webview *webkit.WebView) {
    for true {
        var url string
        fmt.Scanf("%s", &url)
        fmt.Printf("Read %s\n", url)
        webview.LoadUri(url)
    }
}

func getHtml(webview *webkit.WebView) string {
    js := "document.title=document.documentElement.outerHTML"
    webview.ExecuteScript(js)
    str := webview.GetTitle()
    return str
}

func mwain() {
    flag.Parse()

	gtk.Init(nil)
    webkit.InitWebKit()
    go gtk.Main()

    http.Handle("/grab", http.HandlerFunc(doGrab))
    err := http.ListenAndServe(*addr, nil)
    if err != nil {
        log.Fatal("ListenAndServe:", err)
    }
}
func doGrab(w http.ResponseWriter, req *http.Request) {
    fmt.Printf("Got Html: %s\n", webkit.RenderHtml(HTML_STRING_2, "."))
}

func initWebView() *webkit.WebView {
	gtk.Init(nil)
	webview := webkit.NewWebView()
	webview.Connect("load-error", func() {
        fmt.Printf("Load Error: %s\n", webview.GetUri())
	})
	webview.Connect("onload-event", func() {
        fmt.Printf("Onload Event: %s\n", webview.GetUri())
	})
	webview.Connect("resource-load-finished", func(wv interface{}) {
        fmt.Printf("Resource Load Finished: %v\n", wv)
	})
	webview.Connect("load-committed", func() {
		//entry.SetText(webview.GetUri())
        fmt.Printf("Load Committed: %s\n", webview.GetUri())
	})
	webview.Connect("load-finished", func() {
		//entry.SetText(webview.GetUri())
        fmt.Printf("Load Finished: %s\n", webview.GetUri())
        //time.Sleep(time.Second)
        webview.ExecuteScript("document.title=document.documentElement.innerHTML")
        str := webview.GetTitle()
        fmt.Printf("Html: %s\n", str)
	    gtk.MainQuit()
	})

    webview.LoadHtmlString(HTML_STRING, ".")
	gtk.Main()
    return webview
}
