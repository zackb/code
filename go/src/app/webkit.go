package main

import (
    "grab/webkit"
	"github.com/mattn/go-gtk/gtk"
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

func wmain() {
    webkit.InitWebKit()
    str := webkit.RenderHtml(HTML_STRING_2, "http://google.com/")
    fmt.Printf("HERERERERE\n")
    fmt.Printf("%s\n", str)
}

var c chan string = make(chan string)

func main() {
	gtk.Init(nil)
    webview := webkit.NewWebView()
    /*
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
        fmt.Printf("Load Committed: %s\n", webview.GetUri())
	})
    webview.Connect("load-changed", func() {
        fmt.Printf("Load Changed: %s\n", webview.GetUri())
	})

    */
	webview.Connect("load-finished", func() {
        fmt.Printf("Load Finished: %s\n", webview.GetUri())
        //time.Sleep(3 * time.Second)
        fmt.Printf("%s\n", getHtml(webview))
    })
    /*
    var d int = 0
    for _,s := range sigs {
        webview.Connect(s, func() {
            fmt.Printf("%s finished\n", s)
            if d > 0 {
                c <- s
            }
            d++
        })
    }
    */
    //go webview.LoadString("<html><DIV>Fooe</DIV></html>", "text/html", "utf-8", "")
    webview.LoadUri("http://google.com")
    //fmt.Printf("FFFFFFFFFFFFFF%s\n", getHtml(webview))
    gtk.Main()
    //go gtk.Main()
    //doit(webview)
}

func doit(webview *webkit.WebView) {
    for true {
        var url string
        fmt.Scanf("%s", &url)
        fmt.Printf("Read %s\n", url)
        //webview.LoadUri(url)
        go webview.LoadString("<html><DIV>Fooe</DIV></html>", "text/html", "utf-8", "")
        select {
        case <- c:
            fmt.Printf("DONE\n")
        }
        fmt.Printf("FFFFFFFFFFFFFF%s\n", getHtml(webview))
    }
}


func getHtml(webview *webkit.WebView) string {
    js := "document.title=document.documentElement.outerHTML"
    webview.ExecuteScript(js)
    str := webview.GetMainFrame().GetTitle()
    //str := webview.GetTitle()
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
        title := webview.GetTitle()
        webview.ExecuteScript("document.title=document.documentElement.innerHTML")
        str := webview.GetTitle()
        webview.ExecuteScript("document.title=" + title)
        fmt.Printf("Html: %s\n", str)
	    gtk.MainQuit()
	})

    webview.LoadHtmlString(HTML_STRING, ".")
	gtk.Main()
    return webview
}

var sigs [61]string = [61]string{
"close-web-view",
"console-message",
"context-menu",
"copy-clipboard",
"create-plugin-widget",
"create-web-view",
"cut-clipboard",
"database-quota-exceeded",
"document-load-finished",
"download-requested",
"editing-began",
"editing-ended",
"entering-fullscreen",
"frame-created",
"geolocation-policy-decision-cancelled",
"geolocation-policy-decision-requested",
"hovering-over-link",
"icon-loaded",
"leaving-fullscreen",
"load-committed",
"load-error",
"load-finished",
"load-progress-changed",
"load-started",
"mime-type-policy-decision-requested",
"move-cursor",
"navigation-policy-decision-requested",
"navigation-requested",
"new-window-policy-decision-requested",
"onload-event",
"paste-clipboard",
"populate-popup",
"print-requested",
"redo",
"resource-content-length-received",
"resource-load-failed",
"resource-load-finished",
"resource-request-starting",
"resource-response-received",
"run-file-chooser",
"script-alert",
"script-confirm",
"script-prompt",
"select-all",
"selection-changed",
"should-apply-style",
"should-begin-editing",
"should-change-selected-range",
"should-delete-range",
"should-end-editing",
"should-insert-node",
"should-insert-text",
"should-show-delete-interface-for-element",
"status-bar-text-changed",
"title-changed",
"undo",
"user-changed-contents",
"viewport-attributes-changed",
"viewport-attributes-recompute-requested",
"web-view-ready",
"window-object-cleared"}

