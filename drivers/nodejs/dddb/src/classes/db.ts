import { Socket } from "net"
import Collection from "./collection"
import Model from "./model"
import ResourceRef from "./resourceRef"
import { sendRequest } from "../client"

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

    async getCollections() {
        const res: any = await sendRequest(this.client, { type: "cmd", cmd: "collections", db: this.dbName })
        return res.split("|").filter((it: any) => it !== "")
    }
}

export default Db