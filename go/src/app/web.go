package main

import (
    "grab"
    "flag"
    "html/template"
    "log"
    "net/http"
    "encoding/json"
    "fmt"
)

var addr = flag.String("addr", ":1718", "http service address") // Q=17, R=18

var templ = template.Must(template.New("q").Parse(templateStr))

func main() {
    flag.Parse()
    http.Handle("/", http.HandlerFunc(Index))
    http.Handle("/grab", http.HandlerFunc(doGrab))
    err := http.ListenAndServe(*addr, nil)
    if err != nil {
        log.Fatal("ListenAndServe:", err)
    }
}

func Index(w http.ResponseWriter, req *http.Request) {
    templ.Execute(w, req.FormValue("q"))
}

func doGrab(w http.ResponseWriter, req *http.Request) {
    url := req.FormValue("u")
    grab,_ := grab.GrabUrl(url)
    for k, v := range grab.Html.Meta {
        log.Println("Meta: " + k + " = " + v)
    }

    for k, v := range grab.Headers  {
        log.Println("Header: " + k + " = " + v)
    }

    w.Header().Set("Content-Type", "application/json")
    fmt.Fprint(w, JsonResponse{"item" : grab.Item()})
}

type JsonResponse map[string]interface{}
func (r JsonResponse) String() (s string) {
    b, err := json.Marshal(r)
    if err != nil {
        return ""
    }

    return string(b)
}

const templateStr = `
<html>
<head>
<title>Grab</title>
</head>
<body>
{{if .}}
<img src="http://chart.apis.google.com/chart?chs=300x300&cht=qr&choe=UTF-8&chl={{.}}" />
<br>
{{.}}
<br>
<br>
{{end}}
<form action="/" name=f method="GET"><input maxLength=1024 size=70 name=q value="" title="Text to QR Encode"><input type=submit value="Grab" name=q>
</form>
</body>
</html>
`
