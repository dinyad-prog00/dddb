import { Socket } from "net"
import Collection from "./collection"
import Model from "./model"

class Db {
    client: Socket
    dbName: string

    constructor(client: Socket, dbName: string) {
        this.dbName = dbName
        this.client = client
    }

    collection(name: string) {
        return new Collection(this.client, this.dbName, name)
    }
    model(name: string) {
        return new Model(this.client, this.dbName, name)
    }
}

export default Db