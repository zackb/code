var mysql = require('mysql')

class DB {
    constructor(host, port, username, password) {
        this.con = mysql.createConnection({
            host: "localhost",
            port: 43306,
            user: "meconium",
            password: "meconium"
        });
        this.con.connect(function(err) {
            if (err) throw err;
            console.log("Connected to database");
        });
    }

    list() {
        const sql = "select * from meconium.message"
        return new Promise((resolve, reject) => {
            this.con.query(sql, function (err, result) {
                if (err) {
                    reject(err);
                } else {
                    resolve(result.map(row => ({...row})));
                }
            });
        });
    }
}

const db = new DB("localhost", 43306, "meconium", "meconium");

module.exports = {
    db: db
}
