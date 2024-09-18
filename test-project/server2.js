import pgk from "dddb"
import express, { json } from "express"
const { Dddb } = pgk
const app = express()

const dddb = new Dddb()

const db = dddb.db()

dddb.connect("dddb://admin:yeto20@localhost:33301/test", () => {
    console.log("Connected successfully !")
},
    (err) => {
        console.log("Error de connexion: ", err.message)
    })

app.use(json())

app.post("/books", async (req, res) => {
    const body = req.body;
    const data = await db.model("books").create(body)
    res.send(data)
})

app.listen(3334, () => {
    console.log("Server listening on port 3333")
})
