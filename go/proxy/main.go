package main

import (
	"log"

	hp "github.com/zackb/code/go/proxy/http"
)

func main() {
	s := hp.StartServer(8888)
	log.Fatal(s.ListenAndServeTLS("server.pem", "server.key"))
}
