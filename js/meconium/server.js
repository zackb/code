const express = require('express')
const app = express()
const port = 3000
const { db } = require('./db/db.js')

app.use(express.static("build"));
app.use(express.static("public"));

app.get('/message', (req, res) => {
  db.list().then(function(msgs) {
    res.json({ messages: msgs });
  }).catch((err) => {
    console.log("error", err);
  });
})

app.listen(port, () => {
  console.log(`Meconium listening at http://localhost:${port}`)
})