import { Socket } from "net"
import { CollectionReqPayload, Payload, ReqPayload } from "../types"
import { sendRequest } from "../client"

class Collection {
    client: Socket
    dbName: string
    collectionName: string


    constructor(client: Socket, dbName: string, collectionName: string) {
        this.dbName = dbName
        this.collectionName = collectionName
        this.client = client
    }

    exec(creq: CollectionReqPayload) {
        const req: ReqPayload = { ...creq, type: "req", db: this.dbName, collection: this.collectionName }

        return sendRequest(this.client,req)

    }
}

export default Collection