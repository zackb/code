package main

import (
	"log"
	"net"
	"strconv"

	"github.com/miekg/dns"
)

var domainsToAddresses map[string][]string = map[string][]string{
	"google.com.":  []string{"172.217.164.110"},
	"test.server.": []string{"192.168.1.4", "192.168.1.26"},
}

type handler struct{}

// ServeDNS answeres DNS queries, really poorly
func (h *handler) ServeDNS(w dns.ResponseWriter, r *dns.Msg) {

	msg := dns.Msg{}
	msg.SetReply(r)
	switch r.Question[0].Qtype {
	case dns.TypeA:
		msg.Authoritative = true
		domain := msg.Question[0].Name
		addresses, ok := domainsToAddresses[domain]
		if ok {
			for _, addr := range addresses {
				msg.Answer = append(msg.Answer, &dns.A{
					Hdr: dns.RR_Header{Name: domain, Rrtype: dns.TypeA, Class: dns.ClassINET, Ttl: 60},
					A:   net.ParseIP(addr),
				})
			}
		}
	}
	w.WriteMsg(&msg)
}

func main() {
	srv := &dns.Server{Addr: ":" + strconv.Itoa(53), Net: "udp"}
	srv.Handler = &handler{}
	if err := srv.ListenAndServe(); err != nil {
		log.Fatalf("Failed to set udp listener %s\n", err.Error())
	}
}
