// turn BWZ guestbook csv into basic graphs
// 2019/10/12 @zackb

package main

import (
	"bufio"
	"fmt"
	"io"
	"os"
	"strings"
	"time"

	"github.com/wcharczuk/go-chart"
)

const (
	header = true
	file   = "/Users/zack/Desktop/bwzgb.csv"
)

type person struct {
	date      time.Time
	firstName string
	lastName  string
}

type people []person

func main() {
	ppl := readPeople()
	fmt.Println(ppl)
}

func readPeople() people {
	var res people
	for _, line := range readLines() {
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

func drawGraph() {

	graph := chart.BarChart{
		Title: "BWZ Guestbook",
		Background: chart.Style{
			Padding: chart.Box{
				Top: 40,
			},
		},
		Height:   512,
		BarWidth: 60,
		Bars: []chart.Value{
			{Value: 5.25, Label: "Blue"},
			{Value: 4.88, Label: "Green"},
			{Value: 4.74, Label: "Gray"},
			{Value: 3.22, Label: "Orange"},
			{Value: 3, Label: "Test"},
			{Value: 2.27, Label: "??"},
			{Value: 1, Label: "!!"},
		},
	}

	f, _ := os.Create("output.png")
	defer f.Close()
	graph.Render(chart.PNG, f)
}

func check(e error) {
	if e != nil {
		panic(e)
	}
}

func newPerson(line string) person {
	parts := strings.Split(line, ",")
	return person{
		date:      time.Now(),
		firstName: parts[1],
	}
}
