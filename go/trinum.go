package main

import (
    "fmt"
    "os"
    "strconv"
)

func main() {
    snum := os.Args[1]
    num, _ := strconv.Atoi(snum)

    res := 0
    for i := 1; i <= num; i++ {
        res += i
    }
    fmt.Println(res)
}
