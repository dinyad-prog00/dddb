import pgk from "dddb"
import express, { json } from "express"
const { Dddb } = pgk
const app = express()

const dddb = new Dddb()

const db = dddb.db()

dddb.connect("dddb://localhost:33301/test", () => {
    console.log("Connected successfully !")
})

app.use(json())

app.get("/", async (req, res) => {
    const Task = db.model("books")
    const data = await Task.find({})
    res.send(data)
})

app.get("/:id", async (req, res) => {
    const {id} = req.params
    const data = await db.model("books").findById(id)
    res.send(data)
})

app.listen(3333,()=>{
    console.log("Server listening on port 3333")
})
