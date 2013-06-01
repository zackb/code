package db

import (
    _ "github.com/go-sql-driver/mysql"
    "github.com/lunny/xorm"
    "fmt"
    "time"
)

type GrabResult struct  {
    Url string `xorm:"id pk not null autoincr"`
    StatusCode int
    CreateDate int
}

func G() {
    engine := xorm.Create("mysql", "root:fdsfds@/flix?charset=utf8")
    //err := engine.CreateTables(&GrabResult{})
    err := engine.Map(&GrabResult{})
    if err != nil {
        panic(err)
    }
    err = engine.CreateAll()
    if err != nil {
        panic(err)
    }
    id, err := engine.Insert(&GrabResult{Url:"foo"})
    fmt.Printf("$d - %i\n", id, err)

    res := GrabResult{}
    res.Url = "FFFFFFF"
    res.CreateDate = int(time.Now().Unix())
}
