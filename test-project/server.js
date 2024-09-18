import pgk from "dddb"
import express, { json } from "express"
const { Dddb } = pgk
const app = express()

const dddb = new Dddb()

const db = dddb.db()

dddb.connect("dddb://admin:admin010@localhost:33301/test", () => {
    console.log("Connected successfully !")
},
    (err) => {
        console.log("Error de connexion: ", err.message)
    })

app.use(json())

app.get("/books", async (req, res) => {
    const data = await db.model("books").find({})
    res.send(data)
})


app.get("/sub", async (req, res) => {

    dddb.db("test").ref("books").subscribe((data) => {
        console.log("sub data ============= ", data)
    })
    res.send("Done")
})

app.post("/books/f", async (req, res) => {
    const f = req.body;
    const data = await db.model("books").find(f)
    res.send(data)
})

app.get("/books/:id", async (req, res) => {
    const { id } = req.params
    const data = await db.model("books").findById(id)
    res.send(data)
})

app.post("/books", async (req, res) => {
    const body = req.body;
    const data = await db.model("books").create(body)
    res.send(data)
})

app.listen(3333, () => {
    console.log("Server listening on port 3333")
})
