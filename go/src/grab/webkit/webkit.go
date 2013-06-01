package webkit

import (
	"github.com/mattn/go-gtk/gtk"
    "github.com/mattn/go-gtk/gdk"
    "fmt"
    "time"
)

var webview *WebView
var c chan bool

func InitWebKit() {
	gtk.Init(nil)
    gdk.ThreadsInit()
    c = make(chan bool, 2)
    gdk.ThreadsEnter()
    webview = CreateWebView(func() {
        fmt.Printf("HEEEERLoad Finished\n")
        c <- true
    })
    gdk.ThreadsLeave()

    go func() {
        gdk.ThreadsEnter()
        gtk.Main()
        gdk.ThreadsLeave()
    }()
}

func RenderHtml(html, baseuri string) string {
    result := ""

    fmt.Printf("Starting load\n")
    //go func() {
        gdk.ThreadsEnter()
        webview.GetMainFrame().LoadString(html, "text/html", "UTF-8", baseuri)
        //webview.LoadUri("http://google.com")
        gdk.ThreadsLeave()
        fmt.Printf("Ending load\n")
    //}()

    //for gtk.EventsPending() {
    //    fmt.Printf("Iteration\n")
    //    gtk.MainIteration()
    //}

    timeout := make(chan bool, 1)
    go func() {
        time.Sleep(4 * time.Second)
        timeout <- true
    }()

    select {
        case <- c:
            fmt.Printf("read from channel\n")
        case <- timeout:
            fmt.Printf("timeout\n")
    }
    //gdk.ThreadsEnter()
    result = getHtml(webview)
    //gdk.ThreadsLeave()
    return result
}

func getHtml(webview *WebView) string {
    js := "document.title=document.documentElement.outerHTML"
    webview.ExecuteScript(js)
    str := webview.GetMainFrame().GetTitle()
    return str
}

func CreateWebView(loadfinished func()) *WebView {
    webview := NewWebView()
	webview.Connect("load-error", func() {
        fmt.Printf("Load Error: %s\n", webview.GetUri())
	})
	webview.Connect("user-changed-contents"  , func() {
        fmt.Printf("User changed contents: %s\n", webview.GetUri())
	})
	webview.Connect("onload-event", func() {
        fmt.Printf("Onload Event: %s\n", webview.GetUri())
	})
	webview.Connect("resource-load-finished", func(wv interface{}) {
        fmt.Printf("Resource Load Finished: %v\n", wv)
	})
	webview.Connect("load-committed", func() {
        fmt.Printf("Load Committed: %s\n", webview.GetUri())
	})
	webview.Connect("load-finished", func() {
        fmt.Printf("Load Finished: %s\n", webview.GetUri())
        //loadfinished()
    })
	webview.Connect("window-object-cleared", func() {
        fmt.Printf("Window object cleared: %s\n", webview.GetUri())
        loadfinished()
    })
    return webview
}
