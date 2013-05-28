package grab

import (
    "net/url"
    "strings"
    "bytes"
)

type Item struct {
    Id string
    Title string
    Description string
    ImageUrl string
    Url string
    Tags []string
}

// Reverses a url's domain. Can be better for storage id
// http://bar.foo.com:8983/to/index.html?a=b" becomes "com.foo.bar:http:8983/to/index.html?a=b"
func UrlToId(urlStr string) string {

    url,_ := url.Parse(urlStr)

    parts := strings.Split(url.Host, ":")
    port := ""
    host := parts[0]
    if len(parts) > 1 {
        port = parts[1]
    }

    var buf bytes.Buffer

    // reverse host
    reverseAppendSplits(strings.Split(host, "."), &buf)

    // add protocol
    buf.WriteString(":")
    buf.WriteString(url.Scheme)

    // add port if necessary
    if port != "" {
        buf.WriteString(":")
        buf.WriteString(port)
    }

    // add path
    file := url.Path
    if !strings.HasPrefix(file, "/") {
        buf.WriteString("/")
    }
    buf.WriteString(file)
    //values := url.Query() // no guaranteed order?
    //if len(values) > 0 {
    //    buf.WriteString("?")
    //}
    //for k,v := range values {
    //    buf.WriteString(k + "=" + strings.Join(v,","))
    //}

    //buf.WriteString("?" + url.Query().Encode())
    query := urlStr[strings.Index(urlStr, "?"):]
    buf.WriteString(query)

    return buf.String()
}

func reverseAppendSplits(splits []string, buf *bytes.Buffer) {
    for i := len(splits) - 1; i > 0; i-- {
        buf.WriteString(splits[i] + ".")
    }
    buf.WriteString(splits[0])
}

func IdToUrl(id string) string {
    var buf bytes.Buffer
    path_begin := strings.Index(id, "/")
    if path_begin == -1 {
        path_begin = len(id)
    }
    sub := id[0:path_begin]
    splits := strings.Split(sub, ":")
    buf.WriteString(splits[1]) // scheme
    buf.WriteString("://")
    reverseAppendSplits(strings.Split(splits[0], "."), &buf) // reversed host
    if len(splits) == 3 {
        buf.WriteString(":")
        buf.WriteString(splits[2])
    }
    buf.WriteString(id[path_begin:])
    return buf.String()
}
