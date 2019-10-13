// turn BWZ guestbook csv into basic graphs
// 2019/10/12 @zackb

package main

import (
	"bufio"
	"io"
	"os"
	"sort"
	"strings"
	"time"

	"github.com/chenjiandongx/go-echarts/charts"
)

const (
	header = true
	file   = "/Users/zack/Desktop/bwzgb.csv"
)

type person struct {
	date      time.Time
	firstName string
	lastName  string
	note      string
}

type people []person

type report struct {
	people people
	cp     people
	noncp  people
}

type month struct {
	idx        int
	label      string
	count      int
	cpCount    int
	nonCpCount int
}

type dataGroup map[int]int

func main() {
	r := readReport()
	drawGraph(r)
}

func readReport() report {
	var report report
	for _, p := range readPeople() {
		report.addPerson(p)
	}
	return report
}

func readPeople() people {
	var res people
	for i, line := range readLines() {
		if i == 0 && header {
			continue
		}
		res = append(res, newPerson(line))
	}

	return res
}

func readLines() []string {
	f, err := os.Open(file)
	check(err)
	defer f.Close()

	reader := bufio.NewReader(f)

	var lines []string

	for {
		line, _, err := reader.ReadLine()

		if err == io.EOF {
			break
		}
		lines = append(lines, string(line))
	}

	return lines
}

func check(e error) {
	if e != nil {
		panic(e)
	}
}

func newPerson(line string) person {
	parts := strings.Split(line, ",")
	d, err := time.Parse("1/2/2006", parts[0])
	check(err)
	return person{
		date:      d,
		firstName: parts[1],
		lastName:  parts[2],
		note:      parts[9],
	}
}

func (r *report) addPerson(p person) {
	r.people = append(r.people, p)
	if p.note == "CP" {
		r.cp = append(r.cp, p)
	} else {
		r.noncp = append(r.noncp, p)
	}
}

func (p *person) dateGroupKey() int {
	return 10*int(p.date.Year()) + int(p.date.Month())
}

func (p *person) monthLabel() string {
	return p.date.Format("Jan")
}

func (p *person) monthAndYearLabel() string {
	return p.date.Format("Jan/2006")
}

func (p *people) series() map[string]int {
	s := map[string]int{}

	for _, per := range *p {
		s[per.monthLabel()]++
	}
	return s
}

// counts by year/month
func (p *people) grouped() dataGroup {
	s := dataGroup{}

	for _, per := range *p {
		s[per.dateGroupKey()]++
	}
	return s
}

func (d dataGroup) sort() (index []int) {
	for k := range d {
		index = append(index, k)
	}
	sort.Ints(index)
	return
}

func (r *report) months() []month {
	months := make(map[int]month, len(r.people))

	for _, p := range r.people {
		k := p.dateGroupKey()
		m := months[k]
		m.idx = k
		m.label = p.monthLabel()
		m.count++
		if p.note == "CP" {
			m.cpCount++
		} else {
			m.nonCpCount++
		}
		months[k] = m
	}

	var vals []month
	for _, m := range months {
		vals = append(vals, m)
	}
	sort.Slice(vals, func(i, j int) bool {
		return vals[i].idx < vals[j].idx
	})

	return vals
}

func drawGraph(r report) {
	var labels []string
	var values []int
	var cp []int
	var nocp []int
	for _, m := range r.months() {
		labels = append(labels, m.label)
		values = append(values, m.count)
		cp = append(cp, m.cpCount)
		nocp = append(nocp, m.nonCpCount)
	}

	bar := charts.NewBar()
	bar.SetGlobalOptions(charts.TitleOpts{Title: "BWZ Guestbook"})
	bar.AddXAxis(labels).
		AddYAxis("People", values)
		//AddYAxis("CP", cp).
		//AddYAxis("Not CP", nocp)
	f, err := os.Create("output.html")
	check(err)
	bar.Render(f)
}
