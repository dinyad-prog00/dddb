import { Socket } from "net"
import { CollectionReqPayload, Payload, ReqPayload } from "../types"

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

        return new Promise((resolve, reject) => {
            this.client.write(JSON.stringify(req));

            let dataBuffer = Buffer.from('');
            const onError = (err: any) => {
                reject(err);
                this.client.removeListener('data', onDataReceived)
            }
            const onDataReceived = (data: any) => {

                dataBuffer = Buffer.concat([dataBuffer, data]);
                //console.log("received: ",dataBuffer.toString())
                const jsonData = JSON.parse(dataBuffer.toString());
                if (jsonData.type === "ok") {
                    resolve(jsonData.data);
                } else {
                    reject(jsonData.message);
                }

                this.client.removeListener('data', onDataReceived)
                this.client.removeListener('error', onError);
            };



            this.client.on('data', onDataReceived);

            this.client.on('error', onError);


            const timeout = 5000;
            const timeoutId = setTimeout(() => {
                this.client.removeListener('data', onDataReceived); // Remove the data listener
                this.client.removeListener('error', onError); // Remove the data listener

                reject("Timeout: Data not received within 10s.");
            }, timeout);

            // Clear the timeout if data is received before the timeout expires
            this.client.on('data', () => {
                clearTimeout(timeoutId);
            });
        });

    }
}

export default Collection