package main

import (
    "fmt"
    "os"
    "io"
    "encoding/csv"
    "strconv"
)

var data = map[int]map[int]map[int]int{} // origin[year[month]] = count
var done = make(chan bool)
var out = make(chan []string, 1)

var ID = 0
var ORIGIN = 1
var YEAR = 2
var WEEK = 3

func main() {
    initData()
    go produce("/Users/zack/Desktop/item_origin.csv")
    go consume()
    <- done
    report("/Users/zack/Desktop/item_totals.csv")
}

func report(filename string) {
    f, _ := os.Create(filename)
    defer f.Close()

    // print header
    f.WriteString("Source")
    for y := 2009; y <= 2013; y++ {
        for w := 1; w <= 52; w++ {
            f.WriteString(fmt.Sprintf(",%v-%v", y, w))
        }
    }
    f.WriteString("\n")

    for originId,origin := range origins {
        f.WriteString(origin)
        for y := 2009; y <= 2013; y++ {
            for w := 1; w <= 52; w++ {
                line := fmt.Sprintf(",%v", data[originId][y][w])
                f.WriteString(line)
            }
        }
        f.WriteString("\n")
    }
}

func printData() {
    for origin,yearm := range data {
        for year,weekm := range yearm {
            for week,count := range weekm {
                fmt.Println("Origin: ", origin)
                fmt.Println("Year: ", year)
                fmt.Println("Week: ", week)
                fmt.Println("Count: ", count)
                fmt.Println("\n\n")
            }
        }
    }
}

func consume() {
    for {
        line := <- out
        origin,_ := strconv.Atoi(line[ORIGIN])
        year,_ := strconv.Atoi(line[YEAR])
        week,_ := strconv.Atoi(line[WEEK])
        data[origin][year][week] += 1
    }
}

func initData() {
    for k, _ := range origins {
        data[k] = make(map[int]map[int]int)
        for y := 2009; y <= 2013; y++ {
            data[k][y] = make(map[int]int)
            for w := 1; w <= 52; w++ {
                data[k][y][w] = 0
            }
        }
    }
}

func produce(filename string) {
    file, err := os.Open(filename)
    if err != nil {
        fmt.Println("Error:", err)
        return
    }
    defer file.Close()
    reader := csv.NewReader(file)
    for {
        record, err := reader.Read()
        if err == io.EOF {
            break
        } else if err != nil {
            fmt.Println("Error:", err)
            break
        }
        out <- record
    }
    done <- true
}

func makeKey(origin interface{}, year interface{}, week interface{}) (string) {
    return fmt.Sprintf("%v__%v_%v", origin, year, week)
}

var origins = map[int]string {
    1:"Unknown",
    2:"YouTube",
    3:"CNN",
    4:"Break",
    5:"TweetMeme",
    6:"MSNBC",
    7:"Vimeo",
    8:"IFilm",
    9:"EuroNews",
    10:"Reuters",
    11:"Fox News",
    12:"BBC",
    13:"CBS News",
    14:"Daily Motion",
    15:"ABC News",
    16:"CNBC",
    17:"Engage Media",
    18:"Brave New Films",
    19:"Truveo",
    20:"Twitter",
    21:"Change The Future",
    22:"DailyKosTV",
    23:"Blinkx",
    24:"CastTV",
    25:"Wall Street Journal",
    26:"Yahoo",
    27:"New York Post",
    28:"Vidoosh",
    29:"Slate",
    30:"Trends",
    31:"Digg",
    32:"Frequency",
    33:"SaveVid",
    34:"Funny Or Die",
    35:"OneRiot",
    36:"Google",
    37:"Bing",
    38:"Magma",
    39:"MySpace",
    40:"Metacafe",
    41:"Russia Today",
    42:"ESPN",
    43:"Pitchfork",
    44:"Apple",
    45:"Sky News",
    46:"Blip",
    47:"ITN",
    48:"VOA",
    50:"Aljazeera",
    68:"New York Times",
    79:"MTV",
    80:"Mixes and Mashups",
    81:"Associated Press [YT]",
    82:"Al Jazeera [YT]",
    83:"Comedy Central",
    86:"Entertainment Tonight",
    87:"Baeble Music",
    88:"College Humor",
    89:"5Min",
    90:"Huffington Post",
    91:"Indyish",
    92:"Flix Wagon",
    93:"Sputnik Observatory",
    94:"TMZ",
    95:"Fora TV",
    96:"Muzu TV",
    97:"USA Today",
    98:"Time Life",
    99:"Wired",
    100:"Los Angeles Times",
    101:"Universal Music Group",
    102:"The Daily Beast",
    103:"Style.com",
    104:"A & E",
    105:"All Things Digital",
    106:"ZDNet",
    107:"BabelGum",
    109:"NBC",
    110:"Show Time",
    111:"Life Time",
    112:"Facebook",
    113:"Style.com",
    114:"Viddler",
    115:"Wallpaper.com",
    116:"E! Online",
    117:"AOL Video",
    118:"IMDB",
    119:"Maxim",
    120:"Blender",
    122:"MSN",
    123:"Current",
    124:"CNET",
    125:"LiveLeak",
    126:"Revision3",
    127:"TNT",
    128:"FOX",
    129:"Cracked.com",
    130:"MovieClips",
    131:"The Onion",
    132:"TechCrunch",
    133:"Twitter",
    134:"Hulu",
    135:"VEVO",
    137:"SocialCam",
    138:"Viddy",
    139:"Bonnier",
    140:"POPSUGAR",
    141:"Field & Stream",
    143:"Outdoor Life",
    144:"OutdoorLife",
    145:"TransWorld Motocross",
    146:"TransWorld Ride BMX",
    147:"TransWorld SURF",
    148:"TransWorld Wakeboarding",
    150:"Warren Miller Entertainment",
    151:"Saveur",
    152:"TransWorld Business",
    153:"TransWorld SKATEBOARDING",
    155:"TransWorld SNOWBOARDING",
    156:"Parenting Magazine",
    157:"GrindTV",
    158:"Agence France-Presse",
    159:"Grab Media",
    160:"CraveOnline",
    161:"Vine",
    162:"Hearst Corporation",
    163:"Universal Sports",
    171:"NONE",
    172:"This is a test",
    173:"Instagram",
    174:"WPBF",
    175:"WPTZ",
    176:"WTAE",
    177:"WXII",
    178:"WYFF",
    179:"KHBS/KHOG",
    180:"WGAL",
    181:"KCRA",
    182:"KETV",
    183:"KITV",
    184:"KMBC",
    185:"KOAT",
    186:"KOCO",
    187:"KSBW",
    188:"WAPT",
    189:"WBAL",
    190:"WCVB",
    191:"WDSU",
    192:"WESH",
    193:"KCCI",
    194:"WISN",
    195:"WLKY",
    196:"WLWT",
    197:"WMTW",
    198:"WMUR",
    199:"Cycle World",
    200:"Deutsche Welle",
}
