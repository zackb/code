package grab

import (
    "net/http"
    "net/url"
    "strings"
)
type InMemoryCookieJar struct{
    storage map[string][]http.Cookie
}

func (jar InMemoryCookieJar) SetCookies(u *url.URL, cookies []*http.Cookie) {
	for _, ck := range cookies {
		path := ck.Domain
		if ck.Path != "/" {
			path = ck.Domain + ck.Path
		}
		if len(ck.Domain) > 0 && ck.Domain[0] == '.' {
			path = path[1:]
		}
		if _, found := jar.storage[path]; found {
			setted := false
			for i, v := range jar.storage[path] {
				if v.Name == ck.Name {
                    jar.storage[path][i] = *ck
					setted = true
					break
				}
			}
			if ! setted {
				jar.storage[path] = append(jar.storage[path], *ck)
			}
		} else {
			jar.storage[path] = []http.Cookie{*ck}
		}
	}
}

func (jar InMemoryCookieJar) Cookies(u *url.URL) []*http.Cookie {
	cks := []*http.Cookie{}
	for pattern, cookies := range jar.storage {
		if strings.Contains(u.String(), pattern) {
			for i := range cookies {
				cks = append(cks, &cookies[i])
			}
		}
	}
	return cks
}

func newCookieJar() InMemoryCookieJar {
	storage := make(map[string][]http.Cookie)
	return InMemoryCookieJar{storage}
}
