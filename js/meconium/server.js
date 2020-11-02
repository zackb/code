const express = require('express')
const app = express()
const port = 3000

app.use(express.static("public"));
app.use(express.static("build"));

app.get('/message', (req, res) => {
  res.json({message: "Hello, World!"});
})

app.listen(port, () => {
  console.log(`Meconium listening at http://localhost:${port}`)
})
