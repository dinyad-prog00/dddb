import { Socket } from "net"
import Collection from "./collection"
import Model from "./model"
import ResourceRef from "./resourceRef"

class Db {
    client: Socket
    dbName: string
    uri: string

    constructor(client: Socket, dbName: string, uri?: string) {
        this.dbName = dbName
        this.client = client
        this.uri = uri! ///tbr
    }

    collection(name: string) {
        return new Collection(this.client, this.dbName, name)
    }
    model(name: string) {
        return new Model(this.client, this.dbName, name)
    }

    ref(path: string) {
        return new ResourceRef(this.uri, path)
    }
}

export default Db