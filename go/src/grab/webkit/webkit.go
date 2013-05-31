package webkit

import (
    "fmt"
)

var webview *WebView
var c chan bool

func InitWebKit() {
    c = make(chan bool)
    webview = CreateWebView(func() {
        go loadFinish()
    })
}

func loadFinish() {
    fmt.Printf("HEEEERLoad Finished: %s\n", webview.GetUri())
    c <- true
}

func RenderHtml(html, baseuri string) string {
    result := ""

    go webview.LoadHtmlString(html, baseuri)
    // webview.LoadUri("http://google.com")
    select {
        case <- c:
            fmt.Printf("read from channel\n")
            result = getHtml(webview)
    }
    return result
}

func getHtml(webview *WebView) string {
    js := "document.title=document.documentElement.innerHTML"
    webview.ExecuteScript(js)
    str := webview.GetTitle()
    return str
}

func CreateWebView(loadfinished func()) *WebView {
    webview := NewWebView()
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
	webview.Connect("load-finished", loadfinished)
    return webview
}
